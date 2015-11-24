/* Copyright (C) 2015 J.F.Dockes
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "playctlwidget.h"
#include "HelperStructs/Helper.h"

PlayCtlWidget::PlayCtlWidget(QWidget *parent)
    : PlayCtlWidgetIF(parent), m_playing(false)
{
    setupUi(this);
    connect(btn_play, SIGNAL(clicked()), this, SLOT(onPlayClicked()));
    connect(btn_stop, SIGNAL(clicked()), this, SLOT(onStopClicked()));
    connect(btn_fw, SIGNAL(clicked()), this, SLOT(onForwardClicked()));
    connect(btn_bw, SIGNAL(clicked()), this, SLOT(onBackwardClicked()));
}


// Public slots tell us about the world state
void PlayCtlWidget::onStopped()
{
    btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
}

void PlayCtlWidget::onPaused()
{
    btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
}

void PlayCtlWidget::onPlaying()
{
    btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
}


// Private slots: connected to buttons
void PlayCtlWidget::onPlayClicked()
{
   if (m_playing) {
       btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
       emit pauserequested();
   } else {
       btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
       emit playrequested();
   }

    m_playing = !m_playing;
}

void PlayCtlWidget::onStopClicked()
{
    btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    m_playing = false;

    emit stoprequested();
}

void PlayCtlWidget::onBackwardClicked()
{
    emit backwardrequested();
}

void PlayCtlWidget::onForwardClicked()
{
    emit forwardrequested();
}
