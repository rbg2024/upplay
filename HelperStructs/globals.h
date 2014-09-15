/* Copyright (C) 2012  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT SANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <QString>

#ifndef PREFIX
#define PREFIX "/usr"
#endif

enum AudioState {
    AUDIO_UNKNOWN,
    AUDIO_STOPPED,
    AUDIO_PLAYING,
    AUDIO_PAUSED
};

enum PlaylistAddMode {
    PADM_PLAYNOW = 1, 
    PADM_PLAYNEXT = 2,
    PADM_PLAYLATER = 3,
};

#define DARK_BLUE(x) QString("<font color=#0000FF>") + x + QString("</font>")
#define LIGHT_BLUE(x) QString("<font color=#8888FF>") + x + QString("</font>")

#define PLUGIN_NUM 5
#define PLUGIN_NONE 0
#define PLUGIN_LFM_RADIO 1
#define PLUGIN_STREAM 2
#define PLUGIN_PODCASTS 3
#define PLUGIN_EQUALIZER 4
#define PLUGIN_PLAYLIST_CHOOSER 5
#define PLUGIN_LEVEL 6

namespace Sort {
    enum SortOrder {
        NoSorting=0,
        ArtistNameAsc,
        ArtistNameDesc,
        ArtistTrackcountAsc,
        ArtistTrackcountDesc,
        AlbumNameAsc,
        AlbumNameDesc,
        AlbumYearAsc,
        AlbumYearDesc,
        AlbumTracksAsc,
        AlbumTracksDesc,
        AlbumDurationAsc,
        AlbumDurationDesc,
        TrackNumAsc,
        TrackNumDesc,
        TrackTitleAsc,
        TrackTitleDesc,
        TrackAlbumAsc,
        TrackAlbumDesc,
        TrackArtistAsc,
        TrackArtistDesc,
        TrackYearAsc,
        TrackYearDesc,
        TrackLenghtAsc,
        TrackLengthDesc,
        TrackBitrateAsc,
        TrackBitrateDesc,
        TrackSizeAsc,
        TrackSizeDesc,
        TrackDiscnumberAsc,
        TrackDiscnumberDesc
    };
};

#endif /* GLOBALS_H_ */
