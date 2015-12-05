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
#ifndef PLAYLISTOHRD_H_
#define PLAYLISTOHRD_H_

#include <QDebug>

#include "HelperStructs/MetaData.h"
#include "playlist.h"
#include "upadapt/ohrdadapt.h"

class PlaylistOHRD : public Playlist {
    Q_OBJECT

public:
    // We take ownership of the OHRadio object
    PlaylistOHRD(OHRad *ohrd, QObject *parent = 0);

    virtual ~PlaylistOHRD() {
        delete m_ohrdo;
    }

    virtual void update_state() {
        m_ohrdo->fetchState();
    }
    
signals:
    // All our signals are connected to the OHRad object
    void sig_clear_playlist();
    void sig_insert_tracks(const MetaDataList&, int);
    void sig_tracks_removed(const QList<int>& rows);
    void sig_row_activated(int);

public slots:

    // These receives changes from the remote state.
    void psl_currentTrackId(int id);
    void psl_new_transport_state_impl(int, const char *) {}
    void psl_secs_in_song_impl(quint32) {}
    
    // The following are connected to GUI signals, for responding to
    // user actions.
    void psl_change_track_impl(int idx) {
        qDebug() << "psl_change_track: " << idx;
        emit sig_row_activated(idx);
    }
    void psl_clear_playlist_impl() {}
    void psl_play();
    void psl_pause();
    void psl_stop(); 
    void psl_forward() {}
    void psl_backward() {}
    void psl_remove_rows(const QList<int>&, bool = true) {}
    void psl_insert_tracks(const MetaDataList&, int) {}

    // Set from scratch after reading changes from device
    void psl_new_ohrd(const MetaDataList&);
    void psl_seek(int) {}

private:
    // My link to the OpenHome Renderer
    OHRad *m_ohrdo;
};

#endif /* PLAYLISTOHRD_H_ */
