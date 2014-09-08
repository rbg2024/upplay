/* CSettingsStorage.h */

/* Copyright (C) 2011  Lucio Carreras
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
#ifndef CSettingsStorage_H
#define CSettingsStorage_H

#include <QString>
#include <QSettings>
#include <QApplication>
#include <QPair>
#include <QSize>
#include <QPoint>

#include <vector>

#include <HelperStructs/PlaylistMode.h>
#include <HelperStructs/MetaData.h>

class CSettingsStorage;

using namespace std;

class CSettingsStorage : public QSettings
{
    Q_OBJECT

public:
    static CSettingsStorage* getInstance();

    virtual ~CSettingsStorage ();

    // First time run check
    bool isRunFirstTime ();

private:
    CSettingsStorage(QObject *parent = 0) 
        : QSettings (QCoreApplication::organizationName(),
                     QCoreApplication::applicationName(), parent)
        {
        }

    CSettingsStorage(const CSettingsStorage&);
    CSettingsStorage& operator=(const CSettingsStorage&);

    LastTrack m_lastTrack;
    Playlist_Mode m_playlistmode;
protected:
    void save_all();

public:

    void set_sth_changed(bool);

    bool get_sth_changed();
    QString getVersion();
    void setVersion(QString str);

    QPair<QString,QString> getLastFMNameAndPW ();
    bool getLastFMActive();
    void setLastFMActive(bool b);
    void getLastFMNameAndPW (QString & name, QString & pw);
    void setLastFMNameAndPW (const QString & name,const QString & pw);
    bool getLastFMShowErrors();
    void setLastFMShowErrors(bool b);

    void setLastFMCorrections(bool checked);
    bool getLastFMCorrections();

    int getVolume();
    void setVolume(int vol);

    QSize getPlayerSize();
    void setPlayerSize(QSize size);

    QPoint getPlayerPos();
    void setPlayerPos(QPoint p);

    bool getPlayerFullscreen();
    void setPlayerFullscreen(bool);

    QStringList getPlaylist();
    void setPlaylist(QStringList playlist);

    void setLoadPlaylist(bool b);
    bool getLoadPlaylist();

    bool getLoadLastTrack();
    void setLoadLastTrack(bool b);

    LastTrack* getLastTrack();
    void setLastTrack(LastTrack& t);
    void updateLastTrack();

    bool getRememberTime();
    void setRememberTime(bool);

    bool getStartPlaying();
    void setStartPlaying(bool);

    void setPlaylistMode(const Playlist_Mode& plmode);
    Playlist_Mode getPlaylistMode();

    void setPlayerStyle(int);
    int getPlayerStyle();

    void setShowNotifications(bool);
    bool getShowNotifications();

    void setNotificationTimeout(int);
    int getNotificationTimeout();

    void setNotification(QString);
    QString getNotification();

    void setNotificationScale(int);
    int getNotificationScale();

    void setLastFMSessionKey(QString);
    QString getLastFMSessionKey();

    void setShowLibrary(bool);
    bool getShowLibrary();

    void setLibShownColsTitle(QStringList lst);
    QStringList getLibShownColsTitle();

    void setLibShownColsAlbum(QStringList lst);
    QStringList getLibShownColsAlbum();

    void setLibShownColsArtist(QStringList lst);
    QStringList getLibShownColsArtist();

    void setLibSorting(QList<int> lst);
    QList<int> getLibSorting();

    void setLibShowOnlyTracks(bool);
    bool getLibShowOnlyTracks();

    void setLibLiveSearch(bool);
    bool getLibLiveSheach();

    void setMinimizeToTray(bool);
    bool getMinimizeToTray();

    bool getShowSmallPlaylist();
    void setShowSmallPlaylist(bool);

    QString getSoundEngine();
    void setSoundEngine(QString engine);

    bool getStreamRipper();
    void setStreamRipper(bool b);

    bool getStreamRipperWarning();
    void setStreamRipperWarning(bool b);

    QString getStreamRipperPath();
    void setStreamRipperPath(QString path);

    bool getStreamRipperCompleteTracks();
    void setStreamRipperCompleteTracks(bool b);

    bool getStreamRipperSessionPath();
    void setStreamRipperSessionPath(bool b);

    bool getSocketActivated();
    void setSocketActivated(bool b);

    int getSocketFrom();
    void setSocketFrom(int val);

    int getSocketTo();
    void setSocketTo(int val);

    bool getPlaylistNumbers();
    void setPlaylistNumbers(bool b);

    bool getAllowOnlyOneInstance();
    void setAllowOnlyOneInstance(bool b);

    QString getLanguage();
    void setLanguage(QString lang);

    bool getNotifyNewVersion();
    void setNotifyNewVersion(bool b);
};

#endif // CSettingsStorage_H
