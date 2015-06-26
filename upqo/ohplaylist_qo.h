/* Copyright (C) 2014 J.F.Dockes
 *	 This program is free software; you can redistribute it and/or modify
 *	 it under the terms of the GNU General Public License as published by
 *	 the Free Software Foundation; either version 2 of the License, or
 *	 (at your option) any later version.
 *
 *	 This program is distributed in the hope that it will be useful,
 *	 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *
 *	 You should have received a copy of the GNU General Public License
 *	 along with this program; if not, write to the
 *	 Free Software Foundation, Inc.,
 *	 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef _OHPLAYLIST_QO_INCLUDED
#define _OHPLAYLIST_QO_INCLUDED

#include <string>
#include "libupnpp/config.h"

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDebug>

#include "libupnpp/control/ohplaylist.hxx"
#include "libupnpp/control/cdircontent.hxx"
#include "libupnpp/soaphelp.hxx"

class OHPLMetadata {
public:
    virtual std::string getDidl() const = 0;
};

class OHPlaylistQO : public QObject, public UPnPClient::VarEventReporter {
Q_OBJECT

public:
    OHPlaylistQO(UPnPClient::OHPLH ohp, QObject *parent = 0)
        : QObject(parent), m_curid(-1), m_forceUpdate(false), 
          m_discardArrayEvents(false),
          m_srv(ohp), m_timer(0), m_errcnt(0)
    {
        m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(testconn()));
        m_timer->start(2000);
        qRegisterMetaType<std::vector<int> >("std::vector<int>");
        connect(this, SIGNAL(__idArrayChanged(std::vector<int>)),
                this, SLOT(onIdArrayChanged(std::vector<int>)),
                Qt::QueuedConnection);
        m_srv->installReporter(this);
    }
    virtual ~OHPlaylistQO() {
        m_srv->installReporter(0);
    }
        
    // TransportState, Repeat, Shuffle, Id, TracksMax
    virtual void changed(const char *nm, int value)
    {
        //qDebug() << "OHPL: Changed: " << nm << " (int): " << value;
        if (!strcmp(nm, "Id")) {
            // We could test for an unchanged Id, but let our clients
            // do this because it's quite possible that we emit the
            // first signal before we are connected
            m_curid = value;
            emit trackIdChanged(value);
        } else if (!strcmp(nm, "TransportState")) {
            emit tpStateChanged(value);
        } else if (!strcmp(nm, "Shuffle")) {
            emit shuffleChanged(value != 0);
        } else if (!strcmp(nm, "Repeat")) {
            emit repeatChanged(value != 0);
        }
    }

    // ProtocolInfo
    virtual void changed(const char * /*nm*/, const char * /*value*/)
    {
        //qDebug() << "OHPL: Changed: " << nm << " (char*): " << value;
    }

    // IdArray
    virtual void changed(const char *nm, std::vector<int> ids)
    {
        Q_UNUSED(nm);
        if (!m_discardArrayEvents) {
            //qDebug() << "OHPL: Changed: " << nm << " (vector<int>)";
            emit __idArrayChanged(ids);
        }
    }

public slots:
    virtual void sync() {
        //qDebug() << "OHPL::sync";
        std::vector<int> ids;
        int tp;
        if (idArray(&ids, &tp)) {
            onIdArrayChanged(ids);
        }
    }

    // Ping renderer to check it's still there.
    virtual void testconn() {
        int val;
        if (m_srv->id(&val) != 0) {
            if (m_errcnt++ > 2) {
                emit connectionLost();
            }
            return;
        }
        m_errcnt = 0;
    }

    virtual void asyncArrayUpdates(bool onoff) {
        m_discardArrayEvents = !onoff;
    }

    virtual bool play() {
        //qDebug() << "OHPL::play()";
        return m_srv->play() == 0;
    }
    virtual bool stop() {
        //qDebug() << "OHPL::stop()";
        return m_srv->stop() == 0;
    }
    virtual bool pause() {
        //qDebug() << "OHPL::pause()";
        return m_srv->pause() == 0;
    }
    virtual bool next() {return m_srv->next() == 0;}
    virtual bool previous() {return m_srv->previous() == 0;}
    virtual bool setRepeat(bool b) {return m_srv->setRepeat(b) == 0;}
    virtual bool setShuffle(bool b) {return m_srv->setShuffle(b) == 0;}
    virtual bool seekSecondAbsolute(int s) {
        //qDebug() << "OHPL::seekSecondAbsolute(" << s << ")";
        return m_srv->seekSecondAbsolute(s) == 0;
    }
    virtual bool seekSecondRelative(int s) {
        //qDebug() << "OHPL::sekkSecondRelative()";
        return m_srv->seekSecondRelative(s) == 0;
    }
    virtual bool seekId(int i) {return m_srv->seekId(i) == 0;}
    virtual bool seekIndex(int i) {return m_srv->seekIndex(i) == 0;}
    virtual bool clear() {return m_srv->deleteAll() == 0;}
    virtual bool insert(int afterid, const std::string& uri, 
                        const std::string& didl, int *nid) {
        //qDebug() << "OHPL:: insert after " << afterid;
        int ret = m_srv->insert(afterid, uri, didl, nid);
        if (ret == 0) {
            std::vector<int>::iterator it;
            if (afterid == 0)
                it = m_idsv.begin();
            else 
                it = find(m_idsv.begin(), m_idsv.end(), afterid);
            // Keep local ids vector updated 
            if (it != m_idsv.end()) {
                it++;
                m_idsv.insert(it, *nid);
            }
            // Make sure we get the real stuff
            m_forceUpdate = true;
            return true;
        }
        //qDebug() << "OHPL: insert failed: " << ret;
        return false;
    }

    virtual bool deleteId(int id) {
        if (id == m_curid)
            m_srv->stop();
        int ret = m_srv->deleteId(id);
        if (ret == 0) {
            // Update local state at once in case we get an insert
            // before we get the changed array from the device
            // (drag&drop).
            for (unsigned int i = 0; i < m_idsv.size(); i++) {
                if (m_idsv[i] == id) {
                    m_idsv.erase(m_idsv.begin()+i);
                    break;
                }
            }
            m_metapool.erase(id);
            // Make sure we get the real stuff
            m_forceUpdate = true;
        }
        return ret == 0;
    }

signals:
    void trackIdChanged(int);
    void trackArrayChanged();
    void tpStateChanged(int);
    void shuffleChanged(bool);
    void repeatChanged(bool);
    void connectionLost();

    // This is an internal signal. Use trackArrayChanged()
    void __idArrayChanged(std::vector<int>);
                                         
private slots:

    void onIdArrayChanged(std::vector<int> nids) {
        //qDebug() << "OHPL::onIdArrayChanged: " << vtos(nids).c_str();

        // We used to do nothing if the id array was unchanged, but
        // this gained very little, and going through lets us
        // re-read the current title metadata further on. This could
        // have changed without an id change, for example if the renderer
        // is upmpdcli and mpd is playing an internet radio (no qvers
        // update when the title changes).
#if 0
        if (!m_forceUpdate && nids == m_idsv) {
            //qDebug() << "OHPL::onIdArrayChanged: unchanged";
            return;
        }
#endif
        m_forceUpdate = false;

        // Clean up metapool entries not in ids. We build a set with
        // the new ids list first. For small lists it does not matter,
        // for big ones, this will prevent what would otherwise be a
        // linear search the repeated search to make this
        // quadratic. We're sort of O(n * log(n)) instead.
        if (!m_metapool.empty() && !nids.empty()){
            STD_UNORDERED_SET<int> tmpset(nids.begin(), nids.end());
            for (STD_UNORDERED_MAP<int, UPnPClient::UPnPDirObject>::iterator it
                     = m_metapool.begin(); it != m_metapool.end(); ) {
                if (tmpset.find(it->first) == tmpset.end()) {
                    it = m_metapool.erase(it);
                } else {
                    it++;
                }
            }
        }

        // Find ids for which we have no metadata. Always re-read current title
        std::vector<int> unids; // unknown
        for (std::vector<int>::iterator it = nids.begin(); 
             it != nids.end(); it++) {
            if (m_metapool.find(*it) == m_metapool.end() || m_curid == *it)
                unids.push_back(*it);
        }
        if (!unids.empty()) {
            //qDebug() << "OHPL::onIdArrayChanged: need metadata for: " 
            //       << vtos(unids).c_str();
        }
        // Fetch needed metadata, 10 entries at a time
        const unsigned int batchsize(10);
        for (unsigned int i = 0; i < unids.size();) {
            unsigned int j = 0;
            std::vector<int> metaslice;
            for (; j < batchsize && (i+j) < unids.size(); j++) {
                metaslice.push_back(unids[i+j]);
            }

            //qDebug() << "OHPL::onIdArrayChanged: Requesting metadata for " 
            //<< vtos(metaslice).c_str();
            std::vector<UPnPClient::OHPlaylist::TrackListEntry> entries;
            int ret;
            if ((ret = m_srv->readList(metaslice, &entries))) {
                qDebug() << "OHPL: readList failed: " << ret;
                goto out;
            }
            for (std::vector<UPnPClient::OHPlaylist::TrackListEntry>::iterator
                     it = entries.begin(); it != entries.end(); it++) {
                //qDebug() << "OHPL: data for " << it->id << " " << 
                //    it->dirent.m_title.c_str();
                // Kazoo for example does not set a resource (uri)
                // inside the dirent.  Set it from the uri field in
                // this case.
                if (it->dirent.m_resources.empty()) {
                    UPnPClient::UPnPResource res;
                    res.m_uri = it->url;
                    it->dirent.m_resources.push_back(res);
                }
                m_metapool[it->id] = it->dirent;
            }
            i += j;
        }

        m_idsv = nids;
        qDebug() << "OHPL::onIdArrayChanged: emit trackArrayChanged(). " <<
            "idsv size" << m_idsv.size() << " pool size " << m_metapool.size();
        emit trackArrayChanged();
        emit trackIdChanged(m_curid);
    out:
        return;
    }

protected:
    std::vector<int> m_idsv;
    STD_UNORDERED_MAP<int, UPnPClient::UPnPDirObject> m_metapool;
    int m_curid;
    bool m_forceUpdate;
    bool m_discardArrayEvents;

    std::string vtos(std::vector<int> nids) {
        std::string sids;
        for (std::vector<int>::iterator it = nids.begin(); 
             it != nids.end(); it++)
            sids += UPnPP::SoapHelp::i2s(*it) + " ";
        return sids;
    }


private:
    virtual bool idArray(std::vector<int> *ids, int *tokp) {
        return m_srv->idArray(ids, tokp) == 0;
    }
    UPnPClient::OHPLH m_srv;
    QTimer *m_timer;
    int m_errcnt;
};

#endif // _OHPLAYLIST_QO_INCLUDED
