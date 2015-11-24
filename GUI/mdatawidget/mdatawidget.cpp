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

#include <QDebug>

#include "mdatawidget.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"

MDataWidget::MDataWidget(QWidget *parent)
    : MDataWidgetIF(parent)
{
    setupUi(this);
}

void MDataWidget::setData(const MetaData& md)
{

    // Sometimes ignore the date
    QString albtxt;
    if (md.year < 1000 || md.album.contains(QString::number(md.year))) {
        albtxt = md.album.trimmed();
    } else {
        albtxt = md.album.trimmed() + " (" +
            QString::number(md.year) + ")";
    }
    albtxt = escapeHtml(albtxt);
    lab_album->setText(albtxt);
    lab_album->setToolTip(QString::fromUtf8("<i></i>") + albtxt);

    QString artxt = escapeHtml(md.artist);
    lab_artist->setText(artxt);
    lab_artist->setToolTip(QString::fromUtf8("<i></i>") + artxt);

    QString ttxt = escapeHtml(md.title);
    lab_title->setText(ttxt);
    lab_title->setToolTip(QString::fromUtf8("<i></i>") + ttxt);
}

