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
#include "upadapt/upputils.h"

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
        connect(this, SIGNAL(sig_shuffleState(bool)), 
                this, SLOT(onShuffleState(bool)));
        connect(this, SIGNAL(sig_repeatState(bool)), 
                this, SLOT(onRepeatState(bool)));
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
            udirentToMetadata(&ude, &md);
            md.id = *idit;
            md.pl_playing = md.id == m_curid;
            
            mdv.push_back(md);
        }
        emit metaDataReady(mdv);
    }

    // Insert after idx
    void insertTracks(const MetaDataList& meta, int idx) {
        qDebug() << "OHPlayer::insertTracks: afteridx " << idx;
        if (idx < -1 || idx >= int(m_idsv.size())) {
            return;
        }
        int afterid = idx == -1 ? 0 : m_idsv[idx];
        for (vector<MetaData>::const_iterator it = meta.begin();
             it != meta.end(); it++) {
            if (!insert(afterid, qs2utf8s(it->filepath),
                        qs2utf8s(it->didl), &afterid)) {
                break;
            }
        }
    }

    void removeTracks(const QList<int>& lidx, bool) {
        qDebug() << "OHPlayer::removeTracks";
        for (QList<int>::const_iterator it = lidx.begin(); 
             it != lidx.end(); it++) {
            if (*it >= 0 && *it < int(m_idsv.size())) {
                qDebug() << "OHPlayer::removeTracks: " << m_idsv[*it];
                deleteId(m_idsv[*it]);
            } else {
                qDebug() << "OHPlayer::removeTracks: bad index" << *it;
            }
        }
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
        m_mode = mode;
    }

    void onShuffleState(bool st) {
        m_mode.shuffle = st;
        emit sig_PLModeChanged(m_mode);
    }
    void onRepeatState(bool st) {
        m_mode.repAll = st;
        emit sig_PLModeChanged(m_mode);
    }
signals:
    void sig_audioState(int as, const char *);
    void metaDataReady(const MetaDataList& mdv);
    void sig_PLModeChanged(Playlist_Mode);

private:
    int m_id; // Current playing track
    int m_songsecs;
    Playlist_Mode m_mode;
};


#endif /* _OHPLADAPT_H_INCLUDED_ */
