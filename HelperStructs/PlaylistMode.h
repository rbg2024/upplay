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
#ifndef PLAYLISTMODE_H_
#define PLAYLISTMODE_H_

struct Playlist_Mode {

    // Modes for playing the track sequence: both can be set

    // Restart when done.
    bool repAll;
    // Play in random order.
    bool shuffle;

    // Modes for adding tracks

    // Add at end of list, else after current track
    bool append; 
    // Replace current list. This implies append of course, so
    // grey-out append when this is set
    bool replace; 
    // Begin playing the first track added.
    bool playAdded; 

    Playlist_Mode() { 
        fromInt(0);
    }
    Playlist_Mode(int val) {
        fromInt(val);
    }
    int toInt() const {
        int ret = 0;

        if (repAll)     ret |= 1;
        if (shuffle)    ret |= 2;
        if (append)     ret |= 4;
        if (replace)    ret |= 8;
        if (playAdded)  ret |= 16;

        return ret;
    }
    void fromInt(int val) {
        repAll =      (val & 1) != 0;
        shuffle =     (val & 2) != 0;
        append =      (val & 4) != 0;
        replace =     (val & 8) != 0;
        playAdded =   (val & 16) != 0;
    }
};

#endif
