/* Copyright (C) 2005 J.F.Dockes
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

#include <iostream>
using namespace std;

#include <QWebFrame>
#include <QWebSettings>
#include <QUrl>

#include "libupnpp/log.hxx"

#include "cdbrowser.h"
#include "upputils.h"
#include "dirreader.h"

using namespace UPnPClient;

CDBrowser::CDBrowser(QWidget* parent)
    : QWebView(parent), m_reader(0)
{
    connect(this, SIGNAL(linkClicked(const QUrl &)), 
	    this, SLOT(onLinkClicked(const QUrl &)));
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    setHtml("<html><head><title>Hello World !</title></head>"
            "<body><p>Looking for servers...</p>"
            "</body></html>");
    m_timer.setSingleShot(1);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(serversPage()));
    m_timer.start(0);
}

CDBrowser::~CDBrowser()
{

}

void CDBrowser::appendHtml(const QString& html)
{
    //LOGDEB("CDBrowser::appendHtml: " << qs2utf8s(html) << endl);
    
    QWebFrame *mainframe = page()->mainFrame();
    StringObj morehtml(html);

    mainframe->addToJavaScriptWindowObject("morehtml", &morehtml, 
                                           QScriptEngine::ScriptOwnership);
    QString js("document.body.innerHTML += morehtml.text");
    mainframe->evaluateJavaScript(js);
}

void CDBrowser::onLinkClicked(const QUrl &url)
{
    m_timer.stop();
    string scurl = qs2utf8s(url.toString());
    LOGDEB("CDBrowser::onLinkClicked: " << scurl << endl);

    int what = scurl[0];

    switch (what) {

    // Browse server root
    case 'S':
    {
	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_ctdirs.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad link index: " << i 
                   << " cd count: " << m_ctdirs.size() << endl);
	    return;
	}
        m_cdsidx = i;
	browseContainer(m_cdsidx, "0", "(root)");
    }
    break;

    case 'C':
    {
	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_objids.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad objid index: " << i 
                   << " id count: " << m_objids.size() << endl);
	    return;
	}
	browseContainer(m_cdsidx, m_objids[i].first, m_objids[i].second);
    }
    break;

    case 'L':
    {
	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_curpath.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad curpath index: " << i 
                   << " path count: " << m_curpath.size() << endl);
	    return;
	}
        string objid = m_curpath[i].first;
        string title = m_curpath[i].second;
        m_curpath.erase(m_curpath.begin()+i, m_curpath.end());
	browseContainer(m_cdsidx, objid, title);
    }
    break;

    default:
        LOGERR("CDBrowser::onLinkClicked: bad link type: " << what << endl);
        return;
    }
}

static const QString init_container_page(
    "<html><head>"
    "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"
    "</head><body>"
    "</body></html>"
    );

void CDBrowser::browseContainer(unsigned int cdsidx, string ctid,
                                string cttitle)
{
    LOGDEB("CDBrowser::browseContainer: cdsidx: " << cdsidx << " ctid " 
           << ctid << endl);
    if (cdsidx > m_ctdirs.size()) {
        LOGERR("CDBrowser::browseCT: bad index: " << cdsidx << " cd count: " <<
               m_ctdirs.size() << endl);
        return;
    }
    m_objids.clear();

    m_curpath.push_back(pair<string,string>(ctid, cttitle));

    QString htmlpath("<div class=\"browsepath\">");
    for (unsigned i = 0; i < m_curpath.size(); i++) {
        QString title = QString::fromUtf8(m_curpath[i].second.c_str());
        htmlpath += QString("<a href=\"L%1\">%2</a>/").arg(i).arg(title);
    }
    htmlpath += QString("</div>");

    setHtml(init_container_page);
    appendHtml(htmlpath);

    if (m_reader) {
        delete m_reader;
        m_reader = 0;
    }
    m_reader = new DirReader(this, m_ctdirs[cdsidx], ctid);

    connect(m_reader, SIGNAL(sliceAvailable(const UPnPDirContent *)),
            this, SLOT(onSliceAvailable(const UPnPDirContent *)));
    connect(m_reader, SIGNAL(done(int)), this, SLOT(onDone(int)));
    m_reader->start();
}

void CDBrowser::onDone(int)
{
    LOGDEB("CDBrowser::onDone" << endl);
    if (m_reader) {
        m_reader->wait();
        delete m_reader;
        m_reader = 0;
    }
}

static QString CTToHtml(unsigned int idx, const UPnPDirObject& e)
{
    QString out;
    out += QString("<div class=\"container\" objid=\"%1\">").arg(idx);
    out += QString("<a href=\"C%1\">").arg(idx);
    out += QString::fromUtf8(e.m_title.c_str());
    out += QString("</a></div>");
    return out;
}
static QString ItemToHtml(unsigned int idx, const UPnPDirObject& e)
{
    QString out;
    out += QString("<div class=\"item\" objid=\"%1\">").arg(idx);
    out += QString("<a href=\"I%1\">").arg(idx);
    out += QString::fromUtf8(e.m_title.c_str());
    out += QString("</a></div>");
    return out;
}

void CDBrowser::onSliceAvailable(const UPnPDirContent *dc)
{
    QString html;

    LOGDEB("CDBrowser::onSliceAvailable" << endl);
    for (auto& entry: dc->m_containers) {
        m_objids.push_back(pair<string, string>(entry.m_id, entry.m_title));
        html += CTToHtml(m_objids.size()-1, entry);
    }
    for (auto& entry: dc->m_items) {
        m_objids.push_back(pair<string, string>(entry.m_id, entry.m_title));
        html += ItemToHtml(m_objids.size()-1, entry);
    }
    appendHtml(html);
}

static const string init_server_page(
    "<html><head>"
    "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"
    "</head><body>"
    "<h1>Content Directory Services</h1>"
    "</body></html>"
    );

static QString DSToHtml(unsigned int idx, CDSH cds)
{
    QString out;
    out += QString("<div class=\"cdserver\" cdsid=\"%1\">").arg(idx);
    out += QString("<a href=\"S%1\">").arg(idx);
    out += QString::fromUtf8(cds->getFriendlyName().c_str());
    out += QString("</a></div>");
    return out;
}

void CDBrowser::serversPage()
{
    vector<CDSH> ctdirs;
    if (!ContentDirectory::getServices(ctdirs)) {
        LOGERR("CDBrowser::serversPage: getDirServices failed" << endl);
        return;
    }
    LOGDEB("CDBrowser::serversPage: got " << ctdirs.size() << " services" 
           << endl);
    
    bool same = ctdirs.size() == m_ctdirs.size();
    if (same) {
        for (unsigned i = 0; i < ctdirs.size(); i++) {
            if (ctdirs[i]->getDeviceId().compare(m_ctdirs[i]->getDeviceId())) {
                same = false;
                break;
            }
        }
    }
    if (same) {
        //LOGDEB("CDBrowser::serversPage: no change" << endl);
        m_timer.start(1000);
        return;
    } else {
        LOGDEB1("CDBrowser::serversPage: updating" << endl);
    }
    m_ctdirs = ctdirs;
    setHtml(QString::fromUtf8(init_server_page.c_str()));
    for (unsigned i = 0; i < ctdirs.size(); i++) {
        appendHtml(DSToHtml(i, ctdirs[i]));
    }
    m_timer.start(1000);
}
