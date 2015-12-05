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
#ifndef _OHRADIO_QO_INCLUDED
#define _OHRADIO_QO_INCLUDED

#include <string>

#include <QObject>
#include <QThread>
#include <QDebug>

#include "libupnpp/control/ohradio.hxx"
#include "ohpool.h"

class OHRadioQO : public QObject, public UPnPClient::VarEventReporter {
Q_OBJECT

public:
    OHRadioQO(UPnPClient::OHRDH ohp, QObject *parent = 0)
        : QObject(parent), m_curid(-1), m_srv(ohp) {
        m_srv->installReporter(this);
    }

    virtual ~OHRadioQO() {
        m_srv->installReporter(0);
    }

    // Events: we are interested in:
    //  Id (current playing), TransportState, Shuffle, Repeat.
    //  IdArray (the current playlist. We use readList() to
    //   translate this into actual track information)
    
    virtual void changed(const char *nm, int value) {
        //qDebug() << "OHRD: Changed: " << nm << " (int): " << value;
        if (!strcmp(nm, "Id")) {
            emit currentTrackId(value);
        } else if (!strcmp(nm, "TransportState")) {
            emit tpStateChanged(value);
        }
    }
    virtual void changed(const char *nm, std::vector<int> ids) {
        Q_UNUSED(nm);
        //qDebug() << "OHRD: Changed: " << nm << " (vector<int>)";
        emit __idArrayChanged(ids);
    }
    virtual void changed(const char * /*nm*/, const char * /*value*/) {
        //qDebug() << "OHPL: Changed: " << nm << " (char*): " << value;
    }

public slots:

    // Read state from the remote. Used when starting up, to avoid
    // having to wait for events.
    virtual void fetchState() {
        std::vector<int> ids;
        int tok;
        if (idArray(&ids, &tok))
            onIdArrayChanged(ids);
        if (m_srv->id(&tok) == 0) {
            m_curid = tok;
            emit currentTrackId(m_curid);
        }
        UPnPClient::OHPlaylist::TPState tpst;
        if (m_srv->transportState(&tpst) == 0)
            emit tpStateChanged(tpst);
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

    virtual bool setId(int id) {
        STD_UNORDERED_MAP<int, UPnPClient::UPnPDirObject>::iterator it;
        if ((it = m_metapool.find(id)) == m_metapool.end()) {
            qDebug() << "OHRadioQO::setId: id not found";
            return false;
        }
        const std::string& uri = it->second.m_resources[0].m_uri;
        int ret = m_srv->setId(id, uri);
        if (ret != 0) {
            qDebug() << "OHRadioQO: setId failed: " << ret;
            return false;
        }
        m_curid = id;
        return true;
    }

signals:
    void currentTrackId(int);
    void trackArrayChanged();
    void tpStateChanged(int);

    // This is an internal signal. Use trackArrayChanged()
    void __idArrayChanged(std::vector<int>);

private slots:

    void onIdArrayChanged(std::vector<int> nids) {
        m_idsv = nids;

        if (!ohupdmetapool(nids, m_curid, m_metapool, m_srv))
            return;

        qDebug() << "OHPL::onIdArrayChanged: emit trackArrayChanged(). " <<
            "idsv size" << m_idsv.size() << " pool size " << m_metapool.size();
        emit trackArrayChanged();
        emit currentTrackId(m_curid);
    }
    
protected:
    std::vector<int> m_idsv;
    STD_UNORDERED_MAP<int, UPnPClient::UPnPDirObject> m_metapool;
    int m_curid;

private:
    virtual bool idArray(std::vector<int> *ids, int *tokp) {
        return m_srv->idArray(ids, tokp) == 0;
    }
    UPnPClient::OHRDH m_srv;
};

#endif // _OHRADIO_QO_INCLUDED
