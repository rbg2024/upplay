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

#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CSettingsStorage.h"
#include "Playlist.h"

class PlaylistOH : public Playlist {
    Q_OBJECT

public:
    PlaylistOH(QObject * parent = 0)
        : Playlist(parent)
        {
        }
    virtual ~PlaylistOH() 
        {
        }

public slots:
    // Insert after idx. Use -1 to insert at start
    void psl_new_ohpl(const MetaDataList&);
    virtual void psl_insert_tracks(const MetaDataList&, int) {}
    void psl_append_tracks(const MetaDataList&) {}

    void psl_currentTrack(int id);

    // The following are connected to GUI signals, for responding to
    // user actions.
    void psl_change_track(int) {}
    void psl_next_track() {}
    void psl_prepare_for_end_of_track() {}
    void psl_new_transport_state(int) {}
    void psl_ext_track_change(const QString&) {}
    void psl_playlist_mode_changed(const Playlist_Mode&) {}
    void psl_clear_playlist() {}
    void psl_play();
    void psl_pause();
    void psl_stop(); 
    void psl_forward();
    void psl_backward();
    void psl_remove_rows(const QList<int> &, bool = true)
        {
        }

protected:

    void set_for_playing(int) {}
    void send_next_playing_signal() {}

};

#endif /* PLAYLISTOH_H_ */
