// GUI_PlayerButtons.cpp

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
#include <iostream>
using namespace std;

#include "mainw.h"
#include "trayicon.h"

/** Slots connected to player or trayicon signals **/
void GUI_Player::playClicked()
{
    if (!m_metadata_available) {
        emit play();
        return;
    }

    if (m_playing) {
        emit pause();
    } else {
        emit play();
    }

    m_playing = !m_playing;
    m_trayIcon->setPlaying(m_playing);
    ui->player_w->playctl()->onPlaying();
}

void GUI_Player::stopClicked()
{
    m_trayIcon->setPlaying(false);
    m_trayIcon->stop();
    m_playing = false;

    ui->player_w->playctl()->onStopped();
    
    MetaData md;
    md.title = QString::fromUtf8("Upplay ") + m_settings->getVersion();
    md.artist = m_renderer_friendly_name.isEmpty() ?
        "No renderer connected" :
        tr("Renderer: ") + m_renderer_friendly_name;
    ui->player_w->mdata()->setData(md);

    this->setWindowTitle("Upplay");
    ui->player_w->progress()->setUi(0);

    ui->player_w->albumCover->setIcon(QIcon(Helper::getIconPath() +
                                            "logo.png"));
    emit stop();
}

void GUI_Player::backwardClicked(bool)
{
    // ui->albumCover->setFocus();
    int cur_pos_sec =
        (m_completeLength_ms * ui->player_w->progress()->currentValuePc())
        / 100000;
    if (cur_pos_sec > 3) {
        emit sig_seek(0);
    } else {
        emit backward();
    }
}

void GUI_Player::forwardClicked(bool)
{
    //ui->albumCover->setFocus();
    emit forward();
}


/** PROGRESS BAR **/

void GUI_Player::total_time_changed(qint64 total_time)
{
    m_completeLength_ms = total_time;
    ui->player_w->progress()->setTotalTime(total_time/1000);
}

void GUI_Player::jump_forward()
{
    ui->player_w->progress()->step(1);
}

void GUI_Player::jump_backward()
{
    ui->player_w->progress()->step(-1);
}

// This is called from the external world to update the position
void GUI_Player::setCurrentPosition(quint32 pos_sec)
{
    ui->player_w->progress()->setUi(pos_sec);
}

/** PROGRESS BAR END **/

/** PLAYER BUTTONS END **/



/** VOLUME **/
// Called e.g. after reading saved volume from settings: adjust ui and
// set audio volume.
void GUI_Player::setVolume(int pc)
{
    ui->player_w->volume()->set(pc);
}

// Called from audio when volume has been changed by another player.
void GUI_Player::setVolumeUi(int pc)
{
    if (!m_mute)
        ui->player_w->volume()->setUi(pc);
}

void GUI_Player::setMuteUi(bool ismute)
{
    m_mute = ismute;
    ui->player_w->volume()->setMuteUi(ismute);
}

void GUI_Player::volumeChangedByTick(int val)
{
    int vol_step = m_trayIcon->get_vol_step();
    ui->player_w->volume()->step(vol_step * val);
}

void GUI_Player::volumeHigher()
{
    //qDebug() << "GUI_PLayer::volumeHigher";
    ui->player_w->volume()->volumeHigher();
}

void GUI_Player::volumeLower()
{
    //qDebug() << "GUI_PLayer::volumeLower";
    ui->player_w->volume()->volumeLower();
}

/** VOLUME END **/


