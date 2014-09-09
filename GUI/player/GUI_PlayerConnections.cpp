/* GUI_PlayerConnections.cpp */

/* Copyright (C) 2013  Lucio Carreras
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

void GUI_Player::setupConnections()
{
    connect(ui->btn_play, SIGNAL(clicked(bool)), this,
            SLOT(playClicked(bool)));
    connect(ui->btn_fw, SIGNAL(clicked(bool)), this,
            SLOT(forwardClicked(bool)));
    connect(ui->btn_bw, SIGNAL(clicked(bool)), this,
            SLOT(backwardClicked(bool)));
    connect(ui->btn_stop, SIGNAL(clicked()), this,
            SLOT(stopClicked()));
    connect(ui->btn_mute, SIGNAL(released()), this,
            SLOT(muteButtonPressed()));
    connect(ui->btn_rec, SIGNAL(toggled(bool)), this,
            SLOT(sl_rec_button_toggled(bool)));
    connect(ui->albumCover, SIGNAL(clicked()), this, SLOT(coverClicked()));

    // file
    connect(ui->actionChange_Media_Renderer, SIGNAL(triggered(bool)),
            this, SLOT(onChangeMediaRenderer()));

    connect(ui->action_Close, SIGNAL(triggered(bool)), this,
            SLOT(really_close(bool)));


    // view
    connect(ui->action_viewLibrary, SIGNAL(toggled(bool)), this,
            SLOT(showLibrary(bool)));
    connect(ui->action_Dark, SIGNAL(toggled(bool)), this,
            SLOT(changeSkin(bool)));

    connect(ui->action_smallPlaylistItems, SIGNAL(toggled(bool)), this,
            SLOT(small_playlist_items_toggled(bool)));
    connect(ui->action_Fullscreen, SIGNAL(toggled(bool)), this,
            SLOT(show_fullscreen_toggled(bool)));


    // preferencesF
    connect(ui->action_Language, SIGNAL(triggered(bool)), this,
            SLOT(sl_action_language_toggled(bool)));
    connect(ui->action_min2tray, SIGNAL(toggled(bool)), this,
            SLOT(min2tray_toggled(bool)));
    connect(ui->action_only_one_instance, SIGNAL(toggled(bool)), this,
            SLOT(only_one_instance_toggled(bool)));

    // about
    connect(ui->action_about, SIGNAL(triggered(bool)), this, SLOT(about(bool)));

    connect(ui->action_help, SIGNAL(triggered(bool)), this, SLOT(help(bool)));
    connect(m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)), 
            this, SLOT(volumeChangedByTick(int)));

    connect(ui->volumeSlider, SIGNAL(searchSliderMoved(int)), this,
            SLOT(volumeChanged(int)));
    connect(ui->volumeSlider, SIGNAL(searchSliderReleased(int)), this,
            SLOT(volumeChanged(int)));
    connect(ui->volumeSlider, SIGNAL(searchSliderPressed(int)), this,
            SLOT(volumeChanged(int)));

    connect(ui->songProgress, SIGNAL(searchSliderReleased(int)), this,
            SLOT(setProgressJump(int)));
    connect(ui->songProgress, SIGNAL(searchSliderMoved(int)), this,
            SLOT(setProgressJump(int)));

    QList<QKeySequence> lst;
    lst << QKeySequence(Qt::Key_MediaTogglePlayPause) << QKeySequence(Qt::Key_MediaPlay) << QKeySequence(Qt::Key_MediaPause) << QKeySequence(Qt::Key_Space);
    QAction* play_pause_action = createAction(lst);
    connect(play_pause_action, SIGNAL(triggered()), ui->btn_play, SLOT(click()));

    QList<QKeySequence> lst_fwd;
    lst_fwd << QKeySequence(Qt::Key_MediaNext) << QKeySequence(Qt::ControlModifier | Qt::Key_Right);
    QAction* fwd_action = createAction(lst_fwd);
    connect(fwd_action, SIGNAL(triggered()), ui->btn_fw, SLOT(click()));

    QList<QKeySequence> lst_bwd;
    lst_bwd << QKeySequence(Qt::Key_MediaPrevious) << QKeySequence(Qt::ControlModifier | Qt::Key_Left);
    QAction* bwd_action = createAction(lst_bwd);
    connect(bwd_action, SIGNAL(triggered()), ui->btn_bw, SLOT(click()));

    QAction* stop_action = createAction(QKeySequence(Qt::ControlModifier | Qt::Key_Space));
    connect(stop_action, SIGNAL(triggered()), ui->btn_stop, SLOT(click()));

    QAction* louder_action = createAction(QKeySequence(Qt::AltModifier | Qt::Key_Up));
    connect(louder_action, SIGNAL(triggered()), this, SLOT(volumeHigher()));

    QAction* leiser_action = createAction(QKeySequence(Qt::AltModifier | Qt::Key_Down));
    connect(leiser_action, SIGNAL(triggered()), this, SLOT(volumeLower()));

    QAction* two_perc_plus_action = createAction(QKeySequence(Qt::AltModifier | Qt::Key_Right));
    connect(two_perc_plus_action, SIGNAL(triggered()), this, SLOT(jump_forward()));

    QAction* two_perc_minus_action = createAction(QKeySequence(Qt::AltModifier | Qt::Key_Left));
    connect(two_perc_minus_action, SIGNAL(triggered()), this, SLOT(jump_backward()));

    qDebug() << "connections done";
}

