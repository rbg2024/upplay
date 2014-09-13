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
#ifndef _OHPLADAPT_H_INCLUDED_
#define _OHPLADAPT_H_INCLUDED_
#include <string>
#include <iostream>
using namespace std;

#include <QDebug>
#include "HelperStructs/MetaData.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/PlaylistMode.h"

#include "upqo/ohplaylist_qo.h"
#include "upputils.h"

class OHPlayer : public OHPlaylistQO {
Q_OBJECT

public:
    OHPlayer(UPnPClient::OHPLH ohpl, QObject *parent = 0)
        : OHPlaylistQO(ohpl, parent), m_id(-1), m_songsecs(-1) {
        connect(this, SIGNAL(newTrackArrayReady()),
                this, SLOT(translateMetaData()));
        connect(this, SIGNAL(tpStateChanged(int)), 
                this, SLOT(playerState(int)));
        connect(this, SIGNAL(currentTrack(int)),
                this, SLOT(currentTrack(int)));

    }

    QString u8s2qs(const std::string us) {
        return QString::fromUtf8(us.c_str());
    }

private slots:

    // Seek to time in percent
    void seekPC(int pc) {
        if (m_songsecs == -1) {
            auto poolit = m_metapool.find(m_id);
            if (poolit != m_metapool.end()) {
                UPnPClient::UPnPDirObject& ude = poolit->second;
                std::string sval;
                if (ude.getrprop(0, "duration", sval)) {
                    m_songsecs = UPnPP::upnpdurationtos(sval);
                }
            }
        }

        if (m_songsecs == -1) {
            return;
        }
        int seeksecs = (m_songsecs * pc) / 100;
        seekSecondAbsolute(seeksecs);
    }

    void currentTrack(int id) {
        m_id = id;
        m_songsecs = -1;
    }

    void translateMetaData() {
        qDebug() << "OHPlayer::translateMetaData()";
        MetaDataList mdv;
        for (auto idit = m_idsv.begin(); idit != m_idsv.end(); idit++) {
            auto poolit = m_metapool.find(*idit);
            if (poolit == m_metapool.end()) {
                qDebug() << "OHPlayer::translateMetaData: "
                    "no data found for " << *idit << "!!!";
                continue;
            }
            UPnPClient::UPnPDirObject& ude = poolit->second;
            if (ude.m_resources.empty())
                continue;
            MetaData md;
            md.id = *idit;
            md.title = u8s2qs(ude.m_title);
            md.artist = u8s2qs(ude.m_props["upnp:album"]);
            md.album = u8s2qs(ude.m_props["upnp:artist"].c_str());
            md.rating = 0;
            md.length_ms = 0;
            std::string sval;
            if (ude.getrprop(0, "duration", sval)) {
                md.length_ms = 1000 * UPnPP::upnpdurationtos(sval);
            }
            md.year = 0;
            md.filepath = 
                QString::fromLocal8Bit(ude.m_resources[0].m_uri.c_str());
            md.track_num =  0;
            if (ude.getprop("upnp:originalTrackNumber", sval)) {
                md.track_num = atoi(sval.c_str());
            }
            md.bitrate = 0;
            if (ude.getrprop(0, "bitrate", sval)) {
                md.bitrate = atoi(sval.c_str());
            }
            md.album_id = -1;
            md.artist_id = -1; 
            md.filesize = 0;
            if (ude.getrprop(0, "size", sval)) {
                md.filesize = atoi(sval.c_str());
            }
            md.comment = "";
            md.discnumber = 0;
            sval.clear();
            ude.getprop("upnp:genre", sval);
            md.genres << u8s2qs(sval);
            md.n_discs =  -1;
            md.is_extern = false;
            md.pl_playing = md.id == m_curid;
            md.pl_selected = false;
            md.pl_dragged = false;
            md.is_lib_selected = false;
            md.is_disabled = false;
            md.didl = u8s2qs(ude.getdidl());
            
            mdv.push_back(md);
        }
        emit metaDataReady(mdv);
    }

    void playerState(int ps) {
        std::string s;
        AudioState as = AUDIO_UNKNOWN;
        switch (ps) {
        case UPnPClient::OHPlaylist::TPS_Unknown:
        case UPnPClient::OHPlaylist::TPS_Buffering:
        default:
            s = "Unknown";
            break;
        case UPnPClient::OHPlaylist::TPS_Paused:
            as = AUDIO_PAUSED;
            s = "Paused";
            break;
        case UPnPClient::OHPlaylist::TPS_Playing:
            as = AUDIO_PLAYING;
            s = "Playing";
            break;
        case UPnPClient::OHPlaylist::TPS_Stopped:
            as = AUDIO_STOPPED;
            s = "Stopped";
            break;
        }
        if (as != AUDIO_UNKNOWN) {
            emit sig_audioState(as, s.c_str());
        }
    }

    void  changeMode(Playlist_Mode mode) {
        setRepeat(mode.repAll);
        setShuffle(mode.shuffle);
    }

signals:
    void sig_audioState(int as, const char *);
    void metaDataReady(const MetaDataList& mdv);
private:
    int m_id; // Current playing track
    int m_songsecs;
};


#endif /* _OHPLADAPT_H_INCLUDED_ */
