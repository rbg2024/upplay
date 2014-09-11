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
    virtual ~Playlist();

    uint get_num_tracks();

signals:
    void sig_playlist_updated(MetaDataList&, int, int);
    // This is for player action
    void sig_play_now(const MetaData&, int pos=0, bool play=true);
    // This is for display
    void sig_track_metadata(const MetaData&, int pos=0, bool play=true);
    // We send this when approaching the end of the current track, or
    // if the following track changes. This allows the audio to
    // prepare for gaplessness
    void sig_next_track_to_play(const MetaData&);
    // Inform the GUI about what row to highlight
    void sig_playing_track_changed(int row);
    // Inform about not playing
    void sig_stopped();
    // Try to resume from pause
    void sig_resume_play();

public slots:
    // Create from scratch. Presently unconnected
    void psl_create_playlist(MetaDataList&);
    void psl_insert_tracks(const MetaDataList&, int idx);
    void psl_append_tracks(MetaDataList&);

    void psl_change_track(int);
    void psl_next_track();
    void psl_prepare_for_end_of_track();
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

private:

    MetaDataList _v_meta_data;

    int	 _cur_play_idx;
    bool _is_playing;
    bool _pause;

    Playlist_Mode _playlist_mode;

    CSettingsStorage *_settings;

    void send_cur_playing_signal(int);
    void send_next_playing_signal(int);

    bool checkTrack(const MetaData&) {return true;}
    void remove_row(int row);
};

#endif /* PLAYLIST_H_ */
