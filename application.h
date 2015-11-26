/* application.h */

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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include <QObject>

#include <QApplication>
#include <QStringList>
#include <QMainWindow>
#include <QTranslator>

#include "dirbrowser/dirbrowser.h"
#include "upqo/renderingcontrol_qo.h"
#include "upqo/ohtime_qo.h"
#include "upqo/ohvolume_qo.h"
#include "upadapt/avtadapt.h"
#include "upadapt/ohpladapt.h"

#include "GUI/mainw/mainw.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "playlist/Playlist.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"

class Application : public QObject
{
    Q_OBJECT

    public:
    Application(QApplication* qapp, int n_files, QTranslator* translator, 
                QObject *parent = 0);
    virtual ~Application();

signals:
    
public slots:
    void chooseRenderer();
    void reconnectOrChoose();

private:
    GUI_Player   *m_player;
    Playlist     *m_playlist;
    DirBrowser    *m_cdb;

    RenderingControlQO *m_rdco;
    AVTPlayer    *m_avto;

    OHTimeQO     *m_ohtmo;
    OHVolumeQO   *m_ohvlo;

    GUI_Playlist *m_ui_playlist;

    CSettingsStorage *m_settings;
    QApplication *m_app;

    bool                 m_initialized;

    void init_connections();
    void renderer_connections();
    bool setupRenderer(const std::string& uid);

public:
    void setFiles2Play(QStringList filelist);
    QMainWindow* getMainWindow();
    bool is_initialized();

private:
    QString getVersion();
};


#endif // APPLICATION_H

