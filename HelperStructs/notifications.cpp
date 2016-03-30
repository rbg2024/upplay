/* Copyright (C) 2016 J.F.Dockes
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
#include <string>

#include <QProcess>
#include <QSettings>
#include <QDebug>

#include "upadapt/upputils.h"
#include "GUI/prefs/confgui.h"
#include "notifications.h"

using namespace std;

void UpplayNotifications::notify(const MetaData& meta)
{
    //qDebug() << "UpplayNotifications::notify: title [" << meta.title <<
    //"] artist [" << meta.artist;

    if (meta.title == "") {
        return;
    }
    if (meta.title == m_prevmeta.title && 
        meta.artist == m_prevmeta.artist) {
        return;
    }
    QSettings settings;
    if (!settings.value("shownotifications").toBool()) {
        return;
    }

    m_prevmeta.title = meta.title;
    m_prevmeta.artist = meta.artist;
    
    // Retrieve notification command as string list
    QString cmdstring = settings.value("notificationcmd").toString();
    if (cmdstring.isEmpty()) {
        qDebug() << "UpplayNotifications::notify: no cmd";
        return;
    }
    vector<string> cmdlist;
    confgui::stringToStrings(qs2utf8s(cmdstring), cmdlist);
    if (cmdlist.empty()) {
        qDebug() << "UpplayNotifications::notify: cmd list empty??";
        return;
    }

    QString command = u8s2qs(cmdlist[0]);
    QStringList qlist;
    if (cmdlist.size() > 1) {
        for (unsigned int i = 1; i < cmdlist.size(); i++) {
            qlist.push_back(u8s2qs(cmdlist[i]));
        }
    }

    QString msg = meta.title + " / " + meta.artist;
    qlist.push_back(msg);
    //qDebug() << "UpplayNotifications::notify: starting command " << command;
    QProcess::startDetached(command, qlist, "/tmp");
}
