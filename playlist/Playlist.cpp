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
 *
 */

#include <ctime>
using namespace std;

#include <QList>
#include <QDebug>


#include "Playlist.h"

Playlist::Playlist(QObject * parent) 
    : QObject (parent)
{
    _settings = CSettingsStorage::getInstance();
    _playlist_mode = _settings->getPlaylistMode();
    _v_meta_data.clear();
    _cur_play_idx = -1;
    _pause = false;
}

// remove one row
void Playlist::remove_row(int row)
{
    qDebug() << "Playlist::remove_row";
    QList<int> remove_list;
    remove_list << row;
    psl_remove_rows(remove_list);
}
