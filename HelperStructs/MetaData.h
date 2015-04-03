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

#ifndef METADATA_H_
#define METADATA_H_

#include <vector>
using namespace std;

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QVariant>
#include <QFile>
#include <QDataStream>

#include "HelperStructs/globals.h"


struct MetaData {

    qint32 id;
    qint32 album_id;
    qint32 artist_id;
    QString title;
    QString artist;
    QString album;
    QStringList genres;
    qint32 rating;
    qint64 length_ms;
    qint32 year;
    QString filepath;
    qint32 track_num;
    qint32 bitrate;
    qint64 filesize;
    QString comment;
    int discnumber;
    int n_discs;

    bool is_extern;

    bool pl_selected;
    bool pl_playing;
    bool pl_dragged;

    bool unused1;
    bool is_disabled;

    QString didl;
    QString albumArtURI;

    ///// The following are not saved on disk as part of a saved playlist.///
    // While playing in avt + shuffle mode: remember what we already played
    bool shuffle_played;

    inline MetaData () {
        id = -1;
        artist_id = -1;
        album_id = -1;
        title = "";
        artist = "";
        album = "";
        rating = 0;
        length_ms = 0;
        year = 0;
        filepath = "";
        track_num = 0;
        bitrate = 0;
        is_extern = false;
        filesize = 0;
        comment = "";
        discnumber = 0;
        n_discs = -1;

        pl_selected = false;
        pl_playing = false;
        pl_dragged = false;

        unused1 = false;
        is_disabled = false;

        didl = "";
        
        shuffle_played = false;
    }

    void print() const {

        qDebug() << title
                 << " by " << artist
                 << " from " << album
                 << " (" << length_ms << " m_sec) :: " << filepath;
    }

    QVariant toVariant() const {

        QStringList list;

        QString tmpTitle = title;
        QString tmpArtist = artist;
        QString tmpAlbum = album;

        if (title.trimmed().size() == 0) tmpTitle = QString("(Unknown title)");
        if (artist.trimmed().size() == 0) tmpArtist = QString("");
        if (album.trimmed().size() == 0) tmpAlbum = QString("");

        list.push_back(tmpTitle);
        list.push_back(tmpArtist);
        list.push_back(tmpAlbum);
        list.push_back(QString::number(rating));
        list.push_back(QString::number(length_ms));
        list.push_back(QString::number(year));
        list.push_back(filepath);
        list.push_back(QString::number(track_num));
        list.push_back(QString::number(bitrate));
        list.push_back(QString::number(id));
        list.push_back(QString::number(album_id));
        list.push_back(QString::number(artist_id));
        list.push_back(QString::number(filesize));
        list.push_back(comment);
        list.push_back(QString::number(discnumber));
        list.push_back(QString::number(n_discs));
        list.push_back(genres.join(","));
        list.push_back(QString::number(   (is_extern) ? 1 : 0  ));
        list.push_back( (pl_playing) ? "1" : "0" );
        list.push_back( (pl_selected) ? "1" : "0" );
        list.push_back( (pl_dragged) ? "1" : "0" );
        list.push_back( (unused1) ? "1" : "0" );
        list.push_back( (is_disabled) ? "1" : "0");
        list.push_back(didl);
        list.push_back(albumArtURI);

        return list;
    }

    static bool fromVariant(QVariant v, MetaData& md){

        QStringList list = v.toStringList();

        if(list.size() < 24) return false;
        unsigned int idx = 0;
        md.title =      list[idx++];
        md.artist =     list[idx++];
        md.album =      list[idx++];
        md.rating =     list[idx++].toInt();
        md.length_ms =  list[idx++].toULongLong();
        md.year =       list[idx++].toInt();
        md.filepath =   list[idx++];
        md.track_num =  list[idx++].toInt();
        md.bitrate =    list[idx++].toInt();
        md.id =         list[idx++].toInt();
        md.album_id =   list[idx++].toInt();
        md.artist_id =  list[idx++].toInt();
        md.filesize =   list[idx++].toInt();
        md.comment =    list[idx++];
        md.discnumber = list[idx++].toInt();
        md.n_discs =    list[idx++].toInt();
        md.genres =     list[idx++].split(",");
        md.is_extern = ( list[idx++] == "1" );
        md.pl_playing = (list[idx++] == "1");
        md.pl_selected = (list[idx++] == "1");
        md.pl_dragged = (list[idx++] == "1");
        md.unused1 = (list[idx++] == "1");
        md.is_disabled = (list[idx++] == "1");
        md.didl = list[idx++];
        md.albumArtURI = list[idx++];
        return true;
    }

    bool compare(const MetaData& other) {
        bool same = title == other.title &&
            artist == other.artist &&
            album == other.album &&
            rating == other.rating &&
            length_ms == other.length_ms &&
            year == other.year &&
            filepath == other.filepath &&
            track_num == other.track_num &&
            bitrate == other.bitrate &&
            id == other.id &&
            album_id == other.album_id &&
            artist_id == other.artist_id &&
            filesize == other.filesize &&
            comment == other.comment &&
            discnumber == other.discnumber &&
            n_discs == other.n_discs &&
            genres == other.genres &&
            is_extern == other.is_extern &&
            pl_playing == other.pl_playing &&
            pl_selected == other.pl_selected &&
            pl_dragged == other.pl_dragged &&
            unused1 == other.unused1 &&
            is_disabled == other.is_disabled &&
            didl == other.didl &&
            albumArtURI == other.albumArtURI;
        return !same;
    }

    bool serialize(QDataStream& strm) {
        strm <<
            title << 
            artist << 
            album  << 
            rating << 
            length_ms << 
            year << 
            filepath << 
            track_num << 
            bitrate << 
            id << 
            album_id << 
            artist_id << 
            filesize << 
            comment << 
            discnumber << 
            n_discs << 
            genres << 
            is_extern << 
            pl_playing << 
            pl_selected << 
            pl_dragged << 
            unused1 << 
            is_disabled << 
            didl << 
            albumArtURI;
        return true;
    }
    bool unSerialize(QDataStream& strm) {
        strm >> 
            title >> 
            artist >> 
            album  >> 
            rating >> 
            length_ms >> 
            year >> 
            filepath >> 
            track_num >> 
            bitrate >> 
            id >> 
            album_id >> 
            artist_id >> 
            filesize >> 
            comment >> 
            discnumber >> 
            n_discs >> 
            genres >> 
            is_extern >> 
            pl_playing >> 
            pl_selected >> 
            pl_dragged >> 
            unused1 >> 
            is_disabled >> 
            didl >>
            albumArtURI;
        return strm.status() == QDataStream::Ok;
    }

};

class MetaDataList : public vector<MetaData> {

public:

    MetaDataList(){}
    ~MetaDataList() {
        clear();
    }

    void setCurPlayTrack(int idx) {
        for (uint i = 0; i < size(); i++) {
            if ((int) i == idx) 
                at(i).pl_playing = true;
            else
                at(i).pl_playing = false;
        }
    }

    bool contains(const MetaData& md, bool cs=false, MetaData **found = 0) {
        if(cs) {
            QString filepath = md.filepath.trimmed();

            for(uint i = 0; i < size(); i++){
                QString filepath2 = at(i).filepath.trimmed();
                if(!filepath.compare(filepath2)) {
                    if (found) 
                        *found = &(at(i));
                    return true;
                }
            }
        } else {
            QString filepath = md.filepath.toLower().trimmed();

            for (uint i = 0; i < size(); i++){
                QString filepath2 = at(i).filepath.toLower().trimmed();
                if(!filepath.compare(filepath2)) {
                    if (found) 
                        *found = &(at(i));
                    return true;
                }
            }
        }
        return false;
    }

    bool serialize(const QString& fn) {
         QFile file(fn);
         file.open(QIODevice::WriteOnly);
         QDataStream out(&file);
         for (unsigned int i = 0; i < size(); i++) {
             at(i).serialize(out);
         }
         return true;
    }

    bool unSerialize(const QString& fn) {
         QFile file(fn);
         file.open(QIODevice::ReadOnly);
         QDataStream in(&file);
         MetaData md;
         while (md.unSerialize(in)) {
             push_back(md);
         }
         return true;
    }
};

#endif /* METADATA_H_ */
