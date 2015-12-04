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

class OHRadioQO : public QObject, public UPnPClient::VarEventReporter {
Q_OBJECT

public:
    OHRadioQO(UPnPClient::OHRDH ohp, QObject *parent = 0)
        : QObject(parent), m_srv(ohp)
    {
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
            // We could test for an unchanged Id, but let our clients
            // do this because it's quite possible that we emit the
            // first signal before we are connected
            m_curid = value;
            emit currentTrackId(value);
        } else if (!strcmp(nm, "TransportState")) {
            emit tpStateChanged(value);
        }
    }
    virtual void changed(const char *nm, std::vector<int> ids) {
        Q_UNUSED(nm);
        if (!m_discardArrayEvents) {
            //qDebug() << "OHRD: Changed: " << nm << " (vector<int>)";
            emit __idArrayChanged(ids);
        }
    }

public slots:

signals:
    void currentTrackId(int);
    void tpStateChanged(int);
    void connectionLost();
    // This is an internal signal. Use trackArrayChanged()
    void __idArrayChanged(std::vector<int>);
                                         
private:
    UPnPClient::OHRDH m_srv;
};

#endif // _OHRADIO_QO_INCLUDED
