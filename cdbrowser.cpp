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
#include "upqo/cdirectory_qo.h"

using namespace UPnPClient;

CDBrowser::CDBrowser(QWidget* parent)
    : QWebView(parent), m_cdsidx(-1), m_reader(0)
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
    delete m_reader;
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
        if (i > m_msdescs.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad link index: " << i 
                   << " cd count: " << m_msdescs.size() << endl);
	    return;
	}
        m_cdsidx = i;
        m_curpath.clear();
        m_curpath.push_back(pair<string,string>("0", "(servers)"));
        m_ms = MSRH(new MediaServer(m_msdescs[m_cdsidx]));
	browseContainer("0", m_msdescs[m_cdsidx].friendlyName);
    }
    break;

    case 'I':
    {
        // Item clicked add to playlist

	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_entries.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad objid index: " << i 
                   << " id count: " << m_entries.size() << endl);
	    return;
	}
        MetaDataList mdl;
        mdl.resize(1);
        udirentToMetadata(&m_entries[i], &mdl[0]);
        emit sig_tracks_for_playlist_available(mdl);
    }
    break;

    case 'C':
    {
        // Container clicked
	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_entries.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad objid index: " << i 
                   << " id count: " << m_entries.size() << endl);
	    return;
	}
	browseContainer(m_entries[i].m_id, m_entries[i].m_title);
    }
    break;

    case 'L':
    {
        // Click in curpath section.
	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_curpath.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad curpath index: " << i 
                   << " path count: " << m_curpath.size() << endl);
	    return;
	}
        string objid = m_curpath[i].first;
        string title = m_curpath[i].second;
        m_curpath.erase(m_curpath.begin()+i, m_curpath.end());
        if (i == 0) {
            m_msdescs.clear();
            serversPage();
        } else if (i == 1) {
            // Server root
            browseContainer("0", title);
        } else {
            browseContainer(objid, title);
        }
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

void CDBrowser::browseContainer(string ctid, string cttitle)
{
    LOGDEB("CDBrowser::browseContainer: " << " ctid " << ctid << endl);
    if (!m_ms) {
        LOGERR("CDBrowser::browseContainer: server not set" << endl);
        return;
    }
    CDSH cds = m_ms->cds();
    if (!cds) {
        LOGERR("Cant reach content directory service" << endl);
        return;
    }
    m_entries.clear();

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
    m_reader = new ContentDirectoryQO(cds, ctid, this);

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
    }
    LOGDEB("CDBrowser::onDone done" << endl);
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
    m_entries.reserve(m_entries.size() + dc->m_containers.size() + 
                      dc->m_items.size());
    for (auto& entry: dc->m_containers) {
        m_entries.push_back(entry);
        html += CTToHtml(m_entries.size()-1, entry);
    }
    for (auto& entry: dc->m_items) {
        m_entries.push_back(entry);
        html += ItemToHtml(m_entries.size()-1, entry);
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

static QString DSToHtml(unsigned int idx, const UPnPDeviceDesc& dev)
{
    QString out;
    out += QString("<div class=\"cdserver\" cdsid=\"%1\">").arg(idx);
    out += QString("<a href=\"S%1\">").arg(idx);
    out += QString::fromUtf8(dev.friendlyName.c_str());
    out += QString("</a></div>");
    return out;
}

void CDBrowser::serversPage()
{
    vector<UPnPDeviceDesc> msdescs;
    if (!MediaServer::getDeviceDescs(msdescs)) {
        LOGERR("CDBrowser::serversPage: getDeviceDescs failed" << endl);
        return;
    }
    LOGDEB("CDBrowser::serversPage: " << msdescs.size() << " servers" << endl);
    
    bool same = msdescs.size() == m_msdescs.size();
    if (same) {
        for (unsigned i = 0; i < msdescs.size(); i++) {
            if (msdescs[i].UDN.compare(m_msdescs[i].UDN)) {
                same = false;
                break;
            }
        }
    }
    if (same) {
        //LOGDEB("CDBrowser::serversPage: no change" << endl);
        m_timer.start(5000);
        return;
    } else {
        LOGDEB1("CDBrowser::serversPage: updating" << endl);
    }
    m_msdescs = msdescs;
    setHtml(QString::fromUtf8(init_server_page.c_str()));
    for (unsigned i = 0; i < msdescs.size(); i++) {
        appendHtml(DSToHtml(i, msdescs[i]));
    }
    m_timer.start(5000);
}
