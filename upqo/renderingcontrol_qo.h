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

#include <QDebug>
#include <QObject>

#include "libupnpp/control/renderingcontrol.hxx"

class RenderingControlQO : public QObject, public UPnPClient::VarEventReporter {
Q_OBJECT

public:
    RenderingControlQO(UPnPClient::RDCH rdc, QObject *parent = 0)
        : QObject(parent), m_srv(rdc)
        {
            m_srv->installReporter(this);
        }

    void changed(const char *nm, int value)
        {
            qDebug() << "RDR: Changed: " << nm << " : " << value << " (int)";
            if (!strcmp(nm, "Volume")) {
                emit volumeChanged(value);
            } else if (!strcmp(nm, "Mute")) {
                emit muteChanged(value);
            }
        }
    void changed(const char *nm, const char *value)
        {
            qDebug() << "RDR: Changed: " << nm << " : " << value << " (char*)";
        }

    int volume() {return m_srv->getVolume();}
    bool mute()  {return m_srv->getMute();}

public slots:
    void setVolume(int vol)
        {
            qDebug() << "RDR::setVolume " << vol;
            m_srv->setVolume(vol);
        }
    void setMute(bool mute)
        {
            m_srv->setMute(mute);
        }

signals:
    void volumeChanged(int);
    void muteChanged(bool);

private:
    UPnPClient::RDCH m_srv;
};

