/* Copyright (C) 2014 J.F.Dockes 
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

#ifndef _CDBROWSER_H_INCLUDED_
#define _CDBROWSER_H_INCLUDED_

#include <vector>
#include <iostream>

#include <QtWebKit/QWebView>
#include <QVariant>
#include <QTimer>

#include "libupnpp/cdirectory.hxx"

class DirReader;
class UPnPDirContent;

class CDBrowser : public QWebView
{
    Q_OBJECT;

 public:
    CDBrowser(QWidget* parent = 0);
    virtual ~CDBrowser();

 public slots:
    virtual void serversPage();
    void onDone(int);
    void browseContainer(unsigned int i, const std::string);
    void onSliceAvailable(const UPnPDirContent *);

 signals:

 protected:

 protected slots:
    virtual void appendHtml(const QString& html);
    virtual void onLinkClicked(const QUrl &);

 private:
    std::vector<ContentDirectoryService> m_ctdirs;
    QTimer m_timer;
    DirReader *m_reader;
    std::vector<std::string> m_objids;
    unsigned int m_cdsidx;
};

// A QObject to hold a QString. Maybe there would be a simpler way to
// do this but anyway... We use javascript to append html to the
// window content when new directory entries arrive. We could include
// the html in a javascript statement:
//     innerhtml += "html text"
// But then we'd have to escape all the double quotes. Instead we store
// the text inside the QObject text property and make the QObject
// available to javascript. The javascript statement becomes 
//       innerhtml += newtext.text
class StringObj : public QObject {
    Q_OBJECT;
    Q_PROPERTY(QString text READ text);
public:
    StringObj(const QString& s)
        : m_s(s)
        { }
    QString text()
        {
            return m_s;
        }
    QString m_s;
};

#endif /* _CDBROWSER_H_INCLUDED_ */
