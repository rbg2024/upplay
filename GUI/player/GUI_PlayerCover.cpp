/* Copyright (C) 2012  Lucio Carreras
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

#include "GUI/player/GUI_Player.h"

#include <QtNetwork/QNetworkReply>
#include <QIcon>
#include <QImageReader>
#include <QImage>

/** COVERS **/
void GUI_Player::coverClicked()
{
    QString searchstring;
    if (m_metadata.album.size() != 0 || m_metadata.artist != 0) {
        searchstring = m_metadata.album + " " + m_metadata.artist;
    } else {
        searchstring = m_metadata.title + " " + m_metadata.artist;
    }

    searchstring = searchstring.trimmed();

    QString targetpath = 
        Helper::get_cover_path(m_metadata.artist, m_metadata.album);

    //m_alternate_covers->start(searchstring, targetpath);

    this->setFocus();
}

void GUI_Player::sl_alternate_cover_available(QString /*target_class*/,
                                              QString coverpath)
{
    QString own_coverpath = 
        Helper::get_cover_path(m_metadata.artist, m_metadata.album);
    if (coverpath != own_coverpath) {
        return;
    }

    ui->albumCover->setIcon(QIcon(coverpath));
}

void GUI_Player::sl_cover_fetch_done(QNetworkReply* reply)
{
    qDebug() << "GUI_Player::sl_cover_fetch_done";
    if (reply->error() != QNetworkReply::NoError)
        return;

    QString smime = 
        reply->header(QNetworkRequest::ContentTypeHeader).toString();

    QByteArray imtype;
    if (!smime.compare("image/png", Qt::CaseInsensitive)) {
        imtype = "PNG";
    } else     if (!smime.compare("image/jpeg", Qt::CaseInsensitive)) {
        imtype = "JPG";
    } else     if (!smime.compare("image/gif", Qt::CaseInsensitive)) {
        imtype = "GIF";
    } else {
        qDebug() << "GUI_Player::sl_cover_fetch_done: unsupported mime type: "<<
            smime;
        return;
    }
    QImageReader reader((QIODevice*)reply, imtype);
    reader.setAutoDetectImageFormat(false);

    QImage image;
    if (!reader.read(&image)) {
        qDebug() << "GUI_Player::sl_vover_fetch_done: image read failed " << 
            reader.errorString();
        return;
    }

    QPixmap pixmap;
    pixmap.convertFromImage(image);
    ui->albumCover->setIcon(QIcon(pixmap));
    ui->albumCover->repaint();
    reply->deleteLater();
}


void GUI_Player::sl_no_cover_available()
{
    QString coverpath = Helper::getIconPath() + "logo.png";
    ui->albumCover->setIcon(QIcon(coverpath));
}

// public slot
// cover was found by CoverLookup
void GUI_Player::covers_found(const QStringList& cover_paths, QString call_id)
{
    Q_UNUSED(cover_paths);
    Q_UNUSED(call_id);
    QString cover_path = Helper::get_cover_path(m_metadata.artist, m_metadata.album);

    /*if(!cover_paths.contains(cover_path)) return;*/
    if (!QFile::exists(cover_path)) {
        return;
    }

    ui->albumCover->setIcon(QIcon(cover_path));
    ui->albumCover->repaint();
}

/** COVER END **/

