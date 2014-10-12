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
#ifndef PLAYLIST_H_
#define PLAYLIST_H_

#include <iostream>

#include <QObject>
#include <QList>
#include <QMap>
#include <QStringList>

#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CSettingsStorage.h"

class Playlist : public QObject {
    Q_OBJECT

public:
    Playlist(QObject * parent=0);
    virtual ~Playlist() {}

    virtual uint get_num_tracks() {
        return m_meta.size();
    }

signals:
    // New playlist (for displaying in ui_playlist)
    void sig_playlist_updated(MetaDataList&, int playidx, int/*unused*/);
    // Inform the ui_playlist about what row to highlight
    void sig_playing_track_changed(int row);
    // Current track info for displaying in the control area
    void sig_track_metadata(const MetaData&, int pos_sec=-1, bool play=true);
    // Make sure we have the latest state of the world
    void sig_sync();

    // These request action from the audio part
    void sig_stop();
    void sig_pause();
    void sig_resume_play();
    void sig_forward();
    void sig_backward();
    // Only connect in OHPlaylist mode
    void sig_mode_changed(Playlist_Mode);

    // These report a state transition of the audio part
    void sig_stopped();
    void sig_paused();
    void sig_playing();

    // Done inserting tracks in the playlist
    void insertDone();

public slots:
    virtual void psl_insert_tracks(const MetaDataList&, int idx) = 0;
    virtual void psl_add_tracks(PlaylistAddMode, bool, const MetaDataList&);

    virtual void psl_change_track(int) = 0;
    virtual void psl_new_transport_state(int, const char *);
    // Mode change requested by UI
    virtual void psl_change_mode(const Playlist_Mode&);
    // Mode change reported by audio (OH)
    virtual void psl_mode_changed(Playlist_Mode);
    virtual void psl_clear_playlist();
    virtual void psl_clear_playlist_impl() = 0;
    virtual void psl_play() = 0;
    virtual void psl_pause() = 0;
    virtual void psl_stop() = 0;
    virtual void psl_forward() = 0;
    virtual void psl_backward() = 0;
    // Note: this needs to be implemented locally even in the OH case
    // because an insert may immediatly follow (d&d), before we get
    // the updated list from the player.
    virtual void psl_remove_rows(const QList<int> &, bool select_next=true);
    virtual void psl_selection_min_row(int);

protected:
    MetaDataList m_meta;

    int	 m_play_idx;
    int  m_selection_min_row;
    int  m_tpstate;
    bool _pause;

    Playlist_Mode _playlist_mode;

    CSettingsStorage *_settings;

    virtual void remove_row(int row);
    virtual bool valid_row(int row) {
        return row >= 0 && row < int(m_meta.size());
    }
};

#endif /* PLAYLIST_H_ */
