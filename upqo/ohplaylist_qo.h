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
#include <unordered_map>

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>

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
        : QObject(parent), m_curid(-1), m_srv(ohp), m_timer(0)
    {
        m_srv->installReporter(this);
        m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
        m_timer->start(1000);
        qRegisterMetaType<std::vector<int> >("std::vector<int>");
        connect(this, SIGNAL(idArrayChanged(std::vector<int>)),
                this, SLOT(onIdArrayChanged(std::vector<int>)),
                Qt::QueuedConnection);
    }
    
    // TransportState, Repeat, Shuffle, Id, TracksMax
    virtual void changed(const char *nm, int value)
    {
        //qDebug() << "OHPL: Changed: " << nm << " (int): " << value;
        if (!strcmp(nm, "Id")) {
            emit currentTrack(value);
            m_curid = value;
        }
    }

    // ProtocolInfo
    virtual void changed(const char */*nm*/, const char */*value*/)
    {
        //qDebug() << "OHPL: Changed: " << nm << " (char*): " << value;
    }

    // IdArray
    virtual void changed(const char *nm, std::vector<int> ids)
    {
        Q_UNUSED(nm);
        //qDebug() << "OHPL: Changed: " << nm << " (vector<int>)";
        emit idArrayChanged(ids);
    }

public slots:
    virtual void play() {m_srv->play();}
    virtual void stop() {m_srv->stop();}
    virtual void pause() {m_srv->pause();}
    virtual void next() {m_srv->next();}
    virtual void previous() {m_srv->previous();}
    virtual void setRepat(bool b) {m_srv->setRepeat(b);}
    virtual void setShuffle(bool b) {m_srv->setShuffle(b);}
    virtual void seekSecondAbsolute(int s) {m_srv->seekSecondAbsolute(s);}
    virtual void seekSecondRelative(int s) {m_srv->seekSecondRelative(s);}
    virtual void seekId(int i) {m_srv->seekId(i);}
    virtual void seekIndex(int i) {m_srv->seekIndex(i);}

    // Called by timer every sec. Do we need it ?
    virtual void update() {}

signals:
    void currentTrack(int);
    void newTrackArrayReady();
    void idArrayChanged(std::vector<int>);
                                         
                                         
private slots:
    std::string vtos(std::vector<int> nids) {
        std::string sids;
        for (auto it = nids.begin(); it != nids.end(); it++)
            sids += UPnPP::SoapHelp::i2s(*it) + " ";
        return sids;
    }

    void onIdArrayChanged(std::vector<int> nids) {
        qDebug() << "OHPL::onIdArrayChanged: " << vtos(nids).c_str();
        if (nids == m_idsv) {
            qDebug() << "OHPL::onIdArrayChanged: unchanged";
            return;
        }
        // Clean up metapool entries not in ids
        for (auto it = m_metapool.begin(); it != m_metapool.end(); ) {
            if (find(nids.begin(), nids.end(), it->first) == nids.end()) {
                it = m_metapool.erase(it);
            } else {
                it++;
            }
        }

        // Find ids for which we have no metadata
        std::vector<int> unids; // unknown
        for (auto it = nids.begin(); it != nids.end(); it++) {
            if (m_metapool.find(*it) == m_metapool.end())
                unids.push_back(*it);
        }
        if (!unids.empty())
            qDebug() << "OHPL: no meta for: " << vtos(unids).c_str();

        // Fetch needed metadata, 10 entries at a time
        const unsigned int batchsize(10);
        for (unsigned int i = 0; i < unids.size();) {
            unsigned int j = 0;
            std::vector<int> small;
            for (; j < batchsize && (i+j) < unids.size(); j++) {
                small.push_back(unids[i+j]);
            }

            //qDebug() << "Requesting metadata for " << vtos(small).c_str();
            std::vector<UPnPClient::OHPlaylist::TrackListEntry> entries;
            int ret;
            if ((ret = m_srv->readList(small, &entries))) {
                qDebug() << "OHPL: readList failed: " << ret;
                goto out;
            }
            for (auto it = entries.begin(); it != entries.end(); it++) {
                //qDebug() << "Data for " << it->id << " " << 
                //    it->dirent.m_title.c_str();
                m_metapool[it->id] = it->dirent;
            }

            i += j;
        }

        m_idsv = nids;
        qDebug() << "EMITTING newTrackArrayReady(). idsv size" 
                 << m_idsv.size() << " pool size " << m_metapool.size();
        emit newTrackArrayReady();

    out:
        return;
    }

protected:
    std::vector<int> m_idsv;
    std::unordered_map<int, UPnPClient::UPnPDirObject> m_metapool;
    int m_curid;

private:
    UPnPClient::OHPLH m_srv;
    QTimer *m_timer;
};

#endif // _OHPLAYLIST_QO_INCLUDED
