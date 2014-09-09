/* Playlist.h */

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
using namespace std;

#include <QObject>
#include <QList>
#include <QMap>
#include <QStringList>

#include "GUI/playlist/model/PlaylistItemModel.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CSettingsStorage.h"


struct BackupPlaylist {
    bool is_valid;
    int cur_play_idx;

    MetaDataList v_md;
};

class Playlist : public QObject {
    Q_OBJECT

public:
    Playlist(QObject * parent=0);
    virtual ~Playlist();

    void load_old_playlist();
    uint get_num_tracks();

signals:
    void sig_playlist_created(MetaDataList&, int, int);
    // This is for player action
    void sig_play_now(const MetaData&, int pos=0, bool play=true);
    // This is for display
    void sig_track_metadata(const MetaData&, int pos=0, bool play=true);
    void sig_next_track_to_play(const MetaData&);
    void sig_selected_file_changed(int row);
    void sig_no_track_to_play();
    void sig_goon_playing();

public slots:
    void psl_createPlaylist(MetaDataList&);
    void psl_insert_tracks(const MetaDataList&, int idx);
    void psl_append_tracks(MetaDataList&);

    void psl_change_track(int);
    void psl_next_track();
    void psl_prepare_for_the_end();
    void psl_new_transport_state(int);
    void psl_ext_track_change(const QString& uri);
    void psl_playlist_mode_changed(const Playlist_Mode&);
    void psl_clear_playlist();
    void psl_play();
    void psl_pause();
    void psl_stop();
    void psl_forward();
    void psl_backward();
    void psl_remove_rows(const QList<int> &, bool select_next_row=true);
    void psl_play_next_tracks(const MetaDataList&);
    void psl_save_playlist_to_storage() {}

private:

    MetaDataList _v_meta_data;

    int	 _cur_play_idx;
    bool _is_playing;
    bool _pause;

    Playlist_Mode _playlist_mode;

    CSettingsStorage *_settings;
    MetaDataList      _v_stream_playlist;
    BackupPlaylist    _backup_playlist;

    void send_cur_playing_signal(int);
    void send_next_playing_signal(int);

    bool checkTrack(const MetaData&) {return true;}
    void remove_row(int row);
};

#endif /* PLAYLIST_H_ */
