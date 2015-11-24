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
#ifndef _PLAYCTLWIDGETIF_H_INCLUDED_
#define _PLAYCTLWIDGETIF_H_INCLUDED_
#include <QWidget>


class PlayCtlWidgetIF : public QWidget {
    Q_OBJECT;

public:
    PlayCtlWidgetIF(QWidget *parent = 0)
        : QWidget(parent) {
    }
    virtual ~PlayCtlWidgetIF() {}

public slots:
    // These are used to set the ui state in sync with externally
    // caused player state changes
    virtual void onStopped() = 0;
    virtual void onPaused() = 0;
    virtual void onPlaying() = 0;

signals:
    void playrequested();
    void pauserequested();
    void stoprequested();
    void backwardrequested();
    void forwardrequested();
};

#endif /* _PLAYCTLWIDGETIF_H_INCLUDED_ */
