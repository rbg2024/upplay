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

#ifndef _MDATAWIDGET_H_INCLUDED_
#define _MDATAWIDGET_H_INCLUDED_

#include "mdatawidgetif.h"
#include "ui_mdatawidget.h"

class MetaData;

class MDataWidget : public MDataWidgetIF, public Ui::MDataWidget {
    Q_OBJECT;

public:
    MDataWidget(QWidget *parent = 0, bool horiz = false);
    virtual ~MDataWidget() {}

public slots:
    virtual void setData(const MetaData& md);
};

class MDataHWidget : public MDataWidget {
public:
    MDataHWidget(QWidget *parent = 0)
        : MDataWidget(parent, true) {
    }
};

#endif /* _MDATAWIDGET_H_INCLUDED_ */
