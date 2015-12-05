/* Copyright (C) 2011  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PLAYLISTOH_H_
#define PLAYLISTOH_H_

#include <iostream>

#include <QObject>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QDebug>

#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CSettingsStorage.h"
#include "playlist.h"
#include "upadapt/ohpladapt.h"

class PlaylistOH : public Playlist {
    Q_OBJECT

public:
    // We take ownership of the OHPlayer object
    PlaylistOH(OHPlayer *ohpl, QObject * parent = 0);

    virtual ~PlaylistOH() {
        delete m_ohplo;
    }

    virtual void update_state() {
        m_ohplo->fetchState();
    }
    
signals:
    // All our signals are connected to the OHPlaylist object
    void sig_clear_playlist();
    void sig_insert_tracks(const MetaDataList&, int);
    void sig_tracks_removed(const QList<int>& rows);
    void sig_row_activated(int);

public slots:

    // These receives changes from the remote state.
    void psl_currentTrackId(int id);
    void psl_new_transport_state_impl(int, const char *);
    void psl_secs_in_song_impl(quint32 s);
    
    // The following are connected to GUI signals, for responding to
    // user actions.
    void psl_change_track_impl(int idx) {
        qDebug() << "psl_change_track: " << idx;
        emit sig_row_activated(idx);
    }
    void psl_clear_playlist_impl();
    void psl_play();
    void psl_pause();
    void psl_stop(); 
    void psl_forward();
    void psl_backward();
    void psl_remove_rows(const QList<int>& rows, bool = true);
    // Insert after idx. Use -1 to insert at start
    void psl_insert_tracks(const MetaDataList&, int);

    // Set from scratch after reading changes from device
    void psl_new_ohpl(const MetaDataList&);
    void psl_seek(int);

private:
    // My link to the OpenHome Renderer
    OHPlayer *m_ohplo;
    // Position in current song, 0 if unknown
    quint32 m_cursongsecs;
    // Current song is last in playlist
    bool m_lastsong;
    // Playing the last 5 S of last song
    bool m_closetoend;
    
    void resetPosState() {
        m_cursongsecs = 0;
        m_lastsong = m_closetoend = false;
    }
};

#endif /* PLAYLISTOH_H_ */
