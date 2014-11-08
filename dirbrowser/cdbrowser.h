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
#include <unordered_set>
#include <set>

#include <QtWebKit/QWebView>
#include <QVariant>
#include <QTimer>
#include <QPoint>

#include "libupnpp/control/description.hxx"
#include "libupnpp/control/mediaserver.hxx"
#include "libupnpp/control/cdirectory.hxx"

#include "HelperStructs/MetaData.h"
#include "HelperStructs/globals.h"

class ContentDirectoryQO;
class RecursiveReaper;
class DirBrowser;

class CDBrowser : public QWebView
{
    Q_OBJECT;

 public:
    CDBrowser(QWidget* parent = 0);
    virtual ~CDBrowser();
    void setDirBrowser(DirBrowser *db) {m_browsers = db;}
    void getSearchCaps(std::set<std::string>& caps) {caps = m_searchcaps;}
    void search(const std::string& iss);

 public slots:
    virtual void serversPage();
    void onBrowseDone(int);
    void onSliceAvailable(const UPnPClient::UPnPDirContent *);
    void onReaperSliceAvailable(const UPnPClient::UPnPDirContent *);
    void setStyleSheet(bool);

 signals:
    void sig_tracks_to_playlist(const MetaDataList&);
    void sig_now_in(QWidget *, const QString&);
    void sig_searchcaps_changed();

 protected slots:
    virtual void appendHtml(const QString&, const QString& html);
    virtual void onLinkClicked(const QUrl &);
    virtual void createPopupMenu(const QPoint&);
    virtual void simpleAdd(QAction *);
    virtual void recursiveAdd(QAction *);
    virtual void back(QAction *);
    virtual void rreaperDone(int);
    virtual void onContentsSizeChanged(const QSize&);

 private:
    void initContainerHtml(const std::string& ss=string());
    void browseContainer(std::string, std::string, QPoint scrollpos = QPoint());
    void curpathClicked(unsigned int i);

    // The currently seen Media Server descriptions
    std::vector<UPnPClient::UPnPDeviceDesc> m_msdescs;
    // Handle for the currently active media server
    UPnPClient::MSRH m_ms;

    // Search caps of current server
    std::set<std::string> m_searchcaps;

    // Current path inside current server: remember objid,title and scroll pos
    struct CtPathElt {
        CtPathElt() : scrollpos(0,0) {}
        CtPathElt(const std::string& id, const std::string& tt)
            : objid(id), title(tt), scrollpos(0,0) 
            {}
        std::string objid;
        std::string title;
        QPoint scrollpos;
    };
    std::vector<CtPathElt> m_curpath;

    // We periodically check the server pool state.
    QTimer m_timer;

    ContentDirectoryQO *m_reader;
    RecursiveReaper    *m_reaper;

    // Content of the currently visited container
    std::vector<UPnPClient::UPnPDirObject> m_entries;
    // Scroll position to be restored when we're done reading
    QPoint m_savedscrollpos;

    // Content of recursive explore
    std::vector<UPnPClient::UPnPDirObject> m_recwalkentries;
    // Store of seen urls hashes for deduplication while walking the tree
    std::unordered_set<std::string> m_recwalkdedup;
    DirBrowser *m_browsers;

    // Objid and index in entries for the last popup menu click
    std::string m_popupobjid;
    int m_popupidx;
    int m_popupmode; // now, next, at end
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
