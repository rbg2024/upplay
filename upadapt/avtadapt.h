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

#ifndef _AVTADAPT_H_INCLUDED_
#define _AVTADAPT_H_INCLUDED_

#include <iostream>
using namespace std;

#include <QDebug>
#include "HelperStructs/MetaData.h"
#include "HelperStructs/globals.h"

#include "upqo/avtransport_qo.h"
#include "upadapt/upputils.h"

class MetaDataAdaptor : public AVTMetadata {
public:
    MetaDataAdaptor(const MetaData &md)
    : m_md(md)
        {}
    virtual ~MetaDataAdaptor() {}

    virtual std::string getDidl() const
        {
            return qs2utf8s(m_md.didl);
        }

private:
    const MetaData& m_md;
};

class AVTPlayer : public AVTransportQO {
Q_OBJECT
public:
    AVTPlayer(UPnPClient::AVTH avt, QObject *parent = 0)
        : AVTransportQO(avt, parent)
        {
            connect(this, SIGNAL(tpStateChanged(int)), 
                    this, SLOT(playerState(int)));
        }

public slots:
    void changeTrack(const MetaData& md, int /*pos_secs*/, bool /*playnow*/)
    {
        MetaDataAdaptor mad(md);
        AVTransportQO::changeTrack(qs2utf8s(md.filepath), &mad);
        AVTransportQO::play();
    }

    // Inform about the next track to play (for setNextAVTURI)
    void infoNextTrack(const MetaData& md)
    {
        MetaDataAdaptor mad(md);
        AVTransportQO::prepareNextTrack(qs2utf8s(md.filepath), &mad);
    }

    void playerState(int tps) {
        std::string s;
        AudioState as = AUDIO_UNKNOWN;
        switch (tps) {
        case UPnPClient::AVTransport::Stopped: 
            s = "Stopped"; as = AUDIO_STOPPED; break;
        case UPnPClient::AVTransport::Playing: 
            s = "Playing"; as = AUDIO_PLAYING; break;
        case UPnPClient::AVTransport::Transitioning: 
            s = "Transitioning"; break;
        case UPnPClient::AVTransport::PausedPlayback: 
            s = "PausedPlayback"; as = AUDIO_PAUSED; break;
        case UPnPClient::AVTransport::PausedRecording: 
            s = "PausedRecording"; as = AUDIO_PAUSED; break;
        case UPnPClient::AVTransport::Recording: 
            s = "Recording"; break;
        case UPnPClient::AVTransport::NoMediaPresent: 
            s = "NoMediaPresent"; as = AUDIO_STOPPED; break;
        case UPnPClient::AVTransport::Unknown: 
        default:
            s = "Unknown"; break;
        }
        if (as != AUDIO_UNKNOWN) {
            emit sig_audioState(as, s.c_str());
        }
    }

signals:
    void sig_audioState(int as, const char *);
};


#endif /* _AVTADAPT_H_INCLUDED_ */
