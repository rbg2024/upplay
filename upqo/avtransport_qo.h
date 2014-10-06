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
#ifndef _AVTRANSPORT_QO_INCLUDED
#define _AVTRANSPORT_QO_INCLUDED

#include <string>

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>

#include "libupnpp/control/avtransport.hxx"
#include "libupnpp/control/cdircontent.hxx"

using namespace UPnPClient;

class AVTMetadata {
public:
    virtual std::string getDidl() const = 0;
};

class AVTransportQO : public QObject, public UPnPClient::VarEventReporter {
Q_OBJECT

public:
    AVTransportQO(UPnPClient::AVTH avt, QObject *parent = 0)
        : QObject(parent), m_srv(avt), m_timer(0), 
          m_cursecs(-1),
          m_sent_end_of_track_sig(false),
          m_in_ending(false)
    {
        m_srv->installReporter(this);
        m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
        m_timer->start(1000);
        qRegisterMetaType<UPnPClient::UPnPDirObject>("UPnPClient::UPnPDirObject");
    }

    virtual void changed(const char *nm, int value)
    {
        if (!strcmp(nm, "CurrentTrackDuration")) {
            //qDebug() << "AVT: Changed: " << nm << " (int): " << value;
            m_cursecs = value;
        } else if (!strcmp(nm, "TransportState")) {
            //qDebug() << "AVT: Changed: " << nm << " (int): " << value;
            emit tpStateChanged(value);
            if (value == UPnPClient::AVTransport::Stopped && m_in_ending) {
                m_in_ending = false;
                // qDebug() << "AVT: stoppedAtEOT";
                emit stoppedAtEOT();
            }
        } else if (!strcmp(nm, "CurrentTransportActions")) {
            //qDebug() << "AVT: Changed: " << nm << " (int): " << value;
            emit tpActionsChanged(value);
        }
    }

    // Note about uri change detection:
    // We use a changed uri to reset the state for sending out a setNextURI
    //
    // This fails if 2 identical URIs are consecutive in the track
    // list. The consequence in this situation is that, if there is a
    // 3rd track and it should be gapless with the second, this won't
    // happen, there will be a stop/start. This does not seem like a
    // big deal.
    //
    // This could be mitigated by adding a time based track-change
    // detection (there is infortunately no sequential "tracks played"
    // counter in UPnP). If the time goes back more than a few seconds
    // without a command from us, then we have a track change.
    virtual void changed(const char *nm, const char *value)
    {
        //qDebug() << "AVT: Changed: " << nm << " (char*): " << value;
        if (!strcmp(nm, "AVTransportURI")) {
            //qDebug() << "AVT: Changed: " << nm << " (char*): " << value;
            if (m_cururi.compare(value)) {
                setcururi(value);
                qDebug() << "AVT: ext track change: " << value;
                emit newTrackPlaying(QString::fromUtf8(value));
            }
        }
    }

    virtual void changed(const char *nm, UPnPClient::UPnPDirObject meta)
    {
        if (!strcmp(nm, "AVTransportURIMetaData")) {
            qDebug() << "AVT: Changed: " << nm << " (dirc): ";
            // meta.dump().c_str();
            emit currentMetadata(meta);
        }
    }

public slots:

    virtual void play() {
        m_srv->play();
    }
    virtual void stop() {
        setcururi("");
        m_srv->stop();
    }
    virtual void pause() {
        m_srv->pause();
    }

    virtual void changeTrack(const std::string& uri, const AVTMetadata* md)
    {
        qDebug() << "AVT:changeTrack: " << uri.c_str();
        m_srv->setAVTransportURI(uri, md->getDidl());
        setcururi(uri);
    }

    virtual void prepareNextTrack(const std::string& uri, const AVTMetadata* md)
    {
        qDebug() << "AVT:prepareNextTrack: " << uri.c_str();
        m_srv->setNextAVTransportURI(uri, md->getDidl());
    }

    // Seek to point. Parameter in percent.
    virtual void seekPC(int pc) 
    {
        qDebug() << "AVT: seekPC " << pc << " %" << " m_cursecs " << m_cursecs;
        if (m_cursecs > 0) {
            m_srv->seek(UPnPClient::AVTransport::SEEK_REL_TIME, 
                        (float(pc)/100.0) * m_cursecs); 
        }
    }

    // Called by timer every sec
    virtual void update() {
        UPnPClient::AVTransport::PositionInfo info;
        if (m_srv->getPositionInfo(info) == 0) {
            emit secsInSongChanged(info.reltime);
        }
        m_cursecs = info.trackduration;
        if (m_cursecs > 0) {
            if (info.reltime > m_cursecs - 10) {
                if (!m_sent_end_of_track_sig) {
                    qDebug() << "Emitting prepare for end";
                    emit endOfTrackIsNear();
                    m_sent_end_of_track_sig = true;
                }
                m_in_ending = true;
            } else if (info.reltime > 0 && info.reltime < 5) {
                if (m_in_ending == true) {
                    qDebug() << "Was in end, seeing start: trackswitch";
                    setcururi(info.trackuri);
                    emit newTrackPlaying(QString::fromUtf8(info.trackuri.c_str()));
                }
            }
        }
    }

signals:
    void secsInSongChanged(quint32);
    void endOfTrackIsNear();
    void newTrackPlaying(QString);
    void tpStateChanged(int);
    void tpActionsChanged(int);
    void stoppedAtEOT();
    void currentMetadata(UPnPClient::UPnPDirObject);

private:
    UPnPClient::AVTH m_srv;
    QTimer *m_timer;
    int m_cursecs;
    bool m_sent_end_of_track_sig;
    bool m_in_ending;
    std::string m_cururi;

    void setcururi(const std::string& uri) {
        qDebug() << "setcururi: " << uri.c_str();
        m_cururi = uri;
        m_sent_end_of_track_sig = false;
        m_in_ending = false;
    }

};

#endif // _AVTRANSPORT_QO_INCLUDED
