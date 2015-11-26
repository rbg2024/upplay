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

#include "volumewidget.h"
#include "HelperStructs/Helper.h"

#include <QDebug>

VolumeWidget::VolumeWidget(QWidget *parent)
    : VolumeWidgetIF(parent)
{
    setupUi(this);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    connect(volumeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onVolumeSliderChanged(int)));
    connect(btn_mute, SIGNAL(clicked()),
            this, SLOT(onButtonClicked()));
    setUi(20);
    setMuteUi(m_mute = false);
}

void VolumeWidget::onVolumeSliderChanged(int value)
{
    setupButton(value);
    emit volumeChanged(value);
}

void VolumeWidget::onButtonClicked()
{
    setMuteUi(!m_mute);
    emit muteChanged(m_mute);
}

void VolumeWidget::set(int value)
{
    setupButton(value);
    volumeSlider->setValueNoSigs(value);
    emit volumeChanged(value);
}

void VolumeWidget::step(int steps)
{
    int current = volumeSlider->value();
    current += steps;
    if (current < 0)
        current = 0;
    if (current > 100)
        current = 100;
    set(current);
}

void VolumeWidget::setUi(int value)
{
    if (!m_mute) {
        setupButton(value);
    }
    volumeSlider->setValueNoSigs(value);
}

void VolumeWidget::setMuteUi(bool ismute)
{
    m_mute = ismute;
    //qDebug() << "VolumeWidget::setMuteUi(" << m_mute << ")";
    volumeSlider->setDisabled(m_mute);
    if (m_mute) {
        btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_mute.png"));
    } else {
        setupButton(volumeSlider->value());
    }
}

void VolumeWidget::setSkinName(const QString& s)
{
    m_skinSuffix = s.isEmpty() ? "" : "_" + s;
    setupButton(volumeSlider->value());
}

void VolumeWidget::setupButton(int value)
{
    //qDebug() << "VolumeWidget::setupButton(" << value << ")";
    QString butFilename = Helper::getIconPath() + "vol_";

    if (value <= 1) {
        butFilename += QString("mute") + m_skinSuffix + ".png";
    } else if (value < 40) {
        butFilename += QString("1") + m_skinSuffix + ".png";
    } else if (value < 80) {
        butFilename += QString("2") + m_skinSuffix + ".png";
    } else {
        butFilename += QString("3") + m_skinSuffix + ".png";
    }

    //qDebug() << "VolumeWidget::setupButton: fn: " << butFilename;
    btn_mute->setIcon(QIcon(butFilename));
}
