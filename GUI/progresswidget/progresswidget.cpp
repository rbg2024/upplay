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
#include "progresswidget.h"

#include <QDebug>

ProgressWidget::ProgressWidget(QWidget *parent = 0)
    : ProgressWidgetIF(parent)
{
}

void ProgressWidget::setTotalTime(int value)
{
}

void ProgressWidget::seek(int value)
{
}

void ProgressWidget::step(int steps)
{
}

void ProgressWidget::set_step_value_pc(int percent)
{
}

void ProgressWidget::set_step_value_secs(int secs)
{
}

void ProgressWidget::setUi(int value)
{
}

void ProgressWidget::onProgressSliderChanged(int value)
{
}

