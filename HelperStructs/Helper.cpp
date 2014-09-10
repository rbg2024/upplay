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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ctime>

#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#include <QDir>
#include <QUrl>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>
#include <QList>
#include <QFile>
#include <QFontMetrics>
#include <QFileInfo>
#include <QMap>
#include <QString>

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CSettingsStorage.h"

QString Helper::cvtQString2FirstUpper(QString str)
{

    QString ret_str = "";
    QStringList lst = str.split(" ");

    foreach(QString word, lst){
        QString first = word.left(1);
        word.remove(0,1);
        word = first.toUpper() + word + " ";

        ret_str += word;
    }

    return ret_str.left(ret_str.size() - 1);
}

QString cvtNum2String(int num, int digits)
{
    QString str = QString::number(num);
    while(str.size() < digits){
        str.prepend("0");
    }

    return str;
}

QString Helper::cvtMsecs2TitleLengthString(long int msec, bool colon, 
                                           bool show_days)
{
    bool show_hrs = false;

    int sec = msec / 1000;
    int min = sec / 60;

    int secs = sec % 60;
    int hrs = min / 60;
    int days = hrs / 24;

    QString final_str;

    if(days > 0 && show_days){
        final_str += QString::number(days) + "d ";
        hrs = hrs % 24;
        show_hrs = true;
    }

    if(!show_days){
        hrs += (days * 24);
    }

    if(hrs > 0 || show_hrs){
        final_str += QString::number(hrs) + "h ";
        min = min % 60;
    }

    if(colon)
        final_str +=  cvtNum2String(min, 2) + ":" + cvtNum2String(secs, 2);
    else
        final_str +=  cvtNum2String(min, 2) + "m " + cvtNum2String(secs, 2);

    return final_str;

}

QString Helper::getSharePath()
{
#warning must use config prefix
    QString path;
#ifndef Q_OS_WIN
    if(QFile::exists("/usr/share/upplay")) 
        path = "/usr/share/upplay/";
    else 
        path = "";
#else
    path = QDir::homePath() + QString("\\.upplay\\images\\");
    if(QFile::exists(path)){
        return path;
    } else 
        path = "";
#endif

    return path;
}

QString Helper::getIconPath()
{
    return ":/icons/";
}

QString Helper::get_cover_path(QString artist, QString album, QString extension)
{
    QString cover_token = calc_cover_token(artist, album);
    QString cover_path =  QDir::homePath() + QDir::separator() + 
        ".upplay" + QDir::separator() + "covers" + QDir::separator() + 
        cover_token + "." + extension;

    if(!QFile::exists(QDir::homePath() + QDir::separator() +".upplay" + 
                      QDir::separator() + "covers")){
        QDir().mkdir(QDir::homePath() + QDir::separator() + ".upplay" + 
                     QDir::separator() + "covers");
    }

    return cover_path;
}

QString Helper::createLink(QString name, QString target, bool underline){
	
    int dark = CSettingsStorage::getInstance()->getPlayerStyle();
    if(target.size() == 0) 
        target = name;

    QString content;
    QString style = "";
	
    if(!underline) 
        style = " style: \"text-decoration=none;\" ";

    if(dark) {
        content = LIGHT_BLUE(name);
    }
    else 
        content = DARK_BLUE(name);
	
    return QString("<a href=\"") + target + "\"" + style + ">" + 
        content + "</a>";
}

QString Helper::calc_filesize_str(qint64 filesize)
{
    qint64 kb = 1024;
    qint64 mb = kb * 1024;
    qint64 gb = mb * 1024;

    QString size;
    if(filesize > gb){
        size = QString::number(filesize / gb) + "." + 
            QString::number((filesize / mb) % gb).left(2)  + " GB";
    } else if (filesize > mb){
        size = QString::number(filesize / mb) + "." + 
            QString::number((filesize / kb) % mb).left(2)  + " MB";
    }  else {
        size = QString::number( filesize / kb) + " KB";
    }

    return size;
}

QString Helper::calc_cover_token(QString artist, QString album)
{
    QString ret = 
        QCryptographicHash::hash(artist.trimmed().toLower().toUtf8() +
                                 album.trimmed().toLower().toUtf8(), 
                                 QCryptographicHash::Md5).toHex();
    return ret;
}

QString Helper::get_album_w_disc(const MetaData& md)
{
    return md.album.trimmed();
}

QStringList Helper::get_soundfile_extensions()
{
    QStringList filters;
    filters << "*.mp3"
            << "*.ogg"
            << "*.oga"
            << "*.m4a"
            << "*.wav"
            << "*.flac"
            << "*.aac"
            << "*.wma";

    QString bla;
    foreach(QString filter, filters) {
        filters.push_back(filter.toUpper());
    }

    return filters;
}

bool Helper::is_soundfile(QString filename)
{
    QStringList extensions = get_soundfile_extensions();
    foreach (QString extension, extensions) {
        if (filename.toLower().endsWith(extension.right(4).toLower())) {
            return true;
        }
    }

    return false;
}

QStringList Helper::get_playlistfile_extensions()
{
    QStringList filters;

    filters << "*.pls"
            << "*.m3u"
            << "*.ram"
            << "*.asx";

    foreach (QString filter, filters) {
        filters.push_back(filter.toUpper());
    }

    return filters;
}

bool Helper::is_playlistfile(QString filename)
{
    QStringList extensions = get_playlistfile_extensions();
    foreach(QString extension, extensions){
        if(filename.toLower().endsWith(extension.right(4).toLower())){
            return true;
        }
    }

    return false;
}


QString Helper::calc_file_extension(QString filename)
{
    int last_point = filename.lastIndexOf(".") + 1;
    return filename.right(filename.size() - last_point);
}

void Helper::remove_files_in_directory(QString dir_name, QStringList filters)
{
    if(filters.size() == 0) 
        filters << "*";

    QDir dir(dir_name);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(filters);
    QStringList file_list = dir.entryList();

    foreach(QString filename, file_list) {
        QFile file(dir.absoluteFilePath(filename));
        file.remove();
    }
}

QString Helper::get_parent_folder(QString filename)
{
    QString ret= filename.left(filename.lastIndexOf(QDir::separator()) + 1);
    int last_idx = ret.lastIndexOf(QDir::separator());
    while(last_idx == ret.size() - 1){
	ret = ret.left(ret.size() - 1);
    	last_idx = ret.lastIndexOf(QDir::separator());
    }
    return ret;
}

QString Helper::get_filename_of_path(QString path)
{
    while (path.endsWith(QDir::separator())) 
        path.remove(path.size() - 1, 1);
    path.remove(Helper::get_parent_folder(path));
    path.remove(QDir::separator());
    return path;
}

void Helper::split_filename(QString src, QString& path, QString& filename)
{
    path = Helper::get_parent_folder(src);
    filename = Helper::get_filename_of_path(src);	
}

QStringList Helper::extract_folders_of_files(QStringList files)
{
    QStringList folders;
    foreach(QString file, files) {
        QString folder = get_parent_folder(file);
        if(!folders.contains(folder))
            folders << folder;
    }

    return folders;
}

bool Helper::read_file_into_str(QString filename, QString* content)
{
    QFile file(filename);
    content->clear();
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }

    while (!file.atEnd()) {
        QByteArray arr = file.readLine();
        QString str = QString::fromLocal8Bit(arr);

        content->append(str);
    }

    file.close();

    if(content->size() > 0 ){
        return true;
    }

    return false;
}

QString Helper::split_string_to_widget(QString str, QWidget* w, QChar sep)
{

    QFontMetrics fm(w->font());

    int width = w->width();


    QString subtext = str;
    QStringList lst;

    while(fm.width(subtext) > width){
        int textsize = fm.width(subtext);
        double scale = (width * 1.0) / textsize;
        int idx = subtext.size() * scale - 2;
        if(idx < 0) idx = 0;

        while(subtext.at(idx) != sep && idx >= 0){
            idx --;
        }

        if(idx >= 0){

            lst << subtext.left(idx+1);
            subtext = subtext.right(subtext.size() - idx);
        }

        else
            break;

    }

    lst << subtext;
    return lst.join("<br />");
}

bool Helper::is_url(QString str){
    if(is_www(str)) return true;
    if(str.startsWith("file"), Qt::CaseInsensitive) return true;
    return false;
}

bool Helper::is_www(QString str){


    if(str.startsWith("http")) return true;
    else if(str.startsWith("ftp")) return true;
    return false;
}

bool Helper::is_dir(QString filename){
    if(!QFile::exists(filename)) return false;
    QFileInfo fileinfo(filename);
    return fileinfo.isDir();
}

bool Helper::is_file(QString filename){
    if(!QFile::exists(filename)) return false;
    QFileInfo fileinfo(filename);
    return fileinfo.isFile();
}


void Helper::set_deja_vu_font(QWidget* w){
    QFont f = w->font();
    f.setFamily("DejaVu Sans");
    f.setStyleStrategy(QFont::PreferAntialias);
    f.setHintingPreference(QFont::PreferNoHinting);
    w->setFont(f);
}
