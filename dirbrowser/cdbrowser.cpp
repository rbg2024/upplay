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
#include <QWebElement>
#include <QMenu>

#include "HelperStructs/Helper.h"

#include "libupnpp/log.hxx"
#include "libupnpp/control/discovery.hxx"

#include "upqo/cdirectory_qo.h"
#include "upadapt/upputils.h"
#include "cdbrowser.h"
#include "rreaper.h"

using namespace UPnPClient;

CDBrowser::CDBrowser(QWidget* parent)
    : QWebView(parent), m_reader(0), m_reaper(0), m_insertactive(false)
{
    connect(this, SIGNAL(linkClicked(const QUrl &)), 
	    this, SLOT(onLinkClicked(const QUrl &)));

    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);

    setHtml("<html><head><title>Upplay directory browser !</title></head>"
            "<body><p>Looking for servers...</p>"
            "</body></html>");

    QString cssurl = QString("file://") + Helper::getSharePath() + 
        "cdbrowser/cdbrowser.css";
    settings()->setUserStyleSheetUrl(cssurl);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
	    this, SLOT(createPopupMenu(const QPoint&)));

    m_timer.setSingleShot(1);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(serversPage()));
    m_timer.start(0);
}

CDBrowser::~CDBrowser()
{
    delete m_reader;
}

void CDBrowser::appendHtml(const QString& elt_id, 
                           const QString& html)
{
    //LOGDEB("CDBrowser::appendHtml: " << qs2utf8s(html) << endl);
    
    QWebFrame *mainframe = page()->mainFrame();
    StringObj morehtml(html);

    mainframe->addToJavaScriptWindowObject("morehtml", &morehtml, 
                                           QScriptEngine::ScriptOwnership);
    QString js;
    if (elt_id.isEmpty()) {
        js = QString("document.body.innerHTML += morehtml.text");
    } else {
        js = QString("document.getElementById(\"%1\").innerHTML += morehtml.text").arg(elt_id);
    }
    mainframe->evaluateJavaScript(js);
}

void CDBrowser::onLinkClicked(const QUrl &url)
{
    m_timer.stop();
    string scurl = qs2utf8s(url.toString());
    //LOGDEB("CDBrowser::onLinkClicked: " << scurl << endl);

    int what = scurl[0];

    switch (what) {

    // Browse server root
    case 'S':
    {
	unsigned int cdsidx = atoi(scurl.c_str()+1);
        if (cdsidx > m_msdescs.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad link index: " << cdsidx 
                   << " cd count: " << m_msdescs.size() << endl);
	    return;
	}
        m_curpath.clear();
        m_curpath.push_back(pair<string,string>("0", "(servers)"));
        m_ms = MSRH(new MediaServer(m_msdescs[cdsidx]));
        if (!m_ms) {
            qDebug() << "MediaServer connect failed";
            return;
        }
	browseContainer("0", m_msdescs[cdsidx].friendlyName);
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
        emit sig_tracks_to_playlist(PADM_PLAYLATER, false, mdl);
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
    qDebug() << "CDBrowser::browseContainer: " << " ctid " << ctid.c_str();
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

    QString htmlpath("<div id=\"browsepath\"><ul>");
    for (unsigned i = 0; i < m_curpath.size(); i++) {
        QString title = QString::fromUtf8(m_curpath[i].second.c_str());
        htmlpath += 
            QString("<li><a href=\"L%1\">%2</a> &gt;</li>").arg(i).arg(title);
    }
    htmlpath += QString("</ul></div><br/>");

    setHtml(init_container_page);
    appendHtml(QString(), htmlpath);
    appendHtml(QString(), "<div id=\"entrylist\"></div>");

    if (m_reader) {
        delete m_reader;
        m_reader = 0;
    }
    m_reader = new ContentDirectoryQO(cds, ctid, this);

    connect(m_reader, SIGNAL(sliceAvailable(const UPnPClient::UPnPDirContent *)),
            this, SLOT(onSliceAvailable(const UPnPClient::UPnPDirContent *)));
    connect(m_reader, SIGNAL(done(int)), this, SLOT(onDone(int)));
    m_reader->start();
}

void CDBrowser::onDone(int)
{
    //qDebug() <<"CDBrowser::onDone";
    if (m_reader) {
        m_reader->wait();
    }
    //qDebug() << "CDBrowser::onDone done";
}

static QString CTToHtml(unsigned int idx, const UPnPDirObject& e)
{
    QString out;
    out += QString("<div class=\"container\" objid=\"%1\" objidx=\"%2\">").
        arg(e.m_id.c_str());
    out += QString("<a class=\"ct_title\" href=\"C%1\">").arg(idx);
    out += QString::fromUtf8(e.m_title.c_str());
    out += QString("</a></div>");
    return out;
}

static QString ItemToHtml(unsigned int idx, const UPnPDirObject& e)
{
    QString out;
    string val;

    out += QString("<div class=\"item\" objid=\"%1\" objidx=\"%2\">").
        arg(e.m_id.c_str()).arg(idx);

    e.getprop("upnp:originalTrackNumber", val);
    out += QString("<span class=\"tracknum\">") + 
        val.c_str() + "</span>";

    out += "<span class=\"tk_title\">";
    out += QString("<a href=\"I%1\">").arg(idx);
    out += QString::fromUtf8(e.m_title.c_str());
    out += "</a>";
    out += "</span>";

    val.clear();
    e.getprop("upnp:artist", val);
    out += "<span class=\"tk_artist\">";
    out += QString::fromUtf8(val.c_str());
    out += "</span>";
    
    val.clear();
    e.getprop("upnp:album", val);
    out += "<span class=\"tk_album\">";
    out += QString::fromUtf8(val.c_str());
    out += "</span>";
    
    val.clear();
    e.getrprop(0, "duration", val);
    int seconds = upnpdurationtos(val);
    // Format as mm:ss
    int mins = seconds / 60;
    int secs = seconds % 60;
    char sdur[100];
    sprintf(sdur, "%02d:%02d", mins, secs);
    out += "<span class=\"tk_duration\">";
    out += sdur;
    out += "</span>";
   

    out += "</div>";

    return out;
}

void CDBrowser::onSliceAvailable(const UPnPDirContent *dc)
{
    QString html;

    qDebug() << "CDBrowser::onSliceAvailable";
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
    appendHtml("entrylist", html);
}

static const string init_server_page(
    "<html><head>"
    "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"
    "</head><body>"
    "<h2 id=\"cdstitle\">Content Directory Services</h2>"
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
    int secs = UPnPDeviceDirectory::getTheDir()->getRemainingDelay();
    if (secs > 1) {
        qDebug() << "CDBrowser::serversPage: waiting " << secs;
        m_timer.start(secs * 1000);
        return;
    }
    if (!MediaServer::getDeviceDescs(msdescs)) {
        LOGERR("CDBrowser::serversPage: getDeviceDescs failed" << endl);
        return;
    }
    //qDebug() << "CDBrowser::serversPage: " << msdescs.size() << " servers";
    
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
        //qDebug() << "CDBrowser::serversPage: updating";
    }
    m_msdescs = msdescs;
    setHtml(QString::fromUtf8(init_server_page.c_str()));
    for (unsigned i = 0; i < msdescs.size(); i++) {
        appendHtml("", DSToHtml(i, msdescs[i]));
    }
    m_timer.start(5000);
}

void CDBrowser::createPopupMenu(const QPoint& pos)
{
    if (m_insertactive)
        return;

    QWebHitTestResult htr = page()->mainFrame()->hitTestContent(pos);
    if (htr.isNull())
	return;
    QWebElement el = htr.enclosingBlockElement();
    while (!el.isNull() && !el.hasAttribute("objid"))
	el = el.parent();
    if (el.isNull())
	return;

    m_popupobjid = qs2utf8s(el.attribute("objid"));
    m_popupidx = el.attribute("objidx").toInt();

    QString otype = el.attribute("class");
    qDebug() << "Popup: " << " class " << otype << " objid " << 
        m_popupobjid.c_str();

    QMenu *popup = new QMenu(this);
    QAction *act;
    QVariant v;
    act = new QAction(tr("Play Now"), this);
    v = QVariant(int(PADM_PLAYNOW));
    act->setData(v);
    popup->addAction(act);

    act = new QAction(tr("Play Next"), this);
    v = QVariant(int(PADM_PLAYNEXT));
    act->setData(v);
    popup->addAction(act);

    act = new QAction(tr("Play Later"), this);
    v = QVariant(int(PADM_PLAYLATER));
    act->setData(v);
    popup->addAction(act);

    if (!otype.compare("container")) {
        popup->connect(popup, SIGNAL(triggered(QAction *)), this, 
                       SLOT(recursiveAdd(QAction *)));
    } else if (!otype.compare("item")) {
        popup->connect(popup, SIGNAL(triggered(QAction *)), this, 
                       SLOT(simpleAdd(QAction *)));
    } else {
        // ??
        qDebug() << "CDBrowser::createPopup: obj type neither ct nor it: " <<
            otype;
        return;
    }
    popup->popup(mapToGlobal(pos));
}

void CDBrowser::simpleAdd(QAction *act)
{
    qDebug() << "CDBrowser::simpleAdd";
    m_popupmode = act->data().toInt();

    if (m_popupidx < 0 || m_popupidx > int(m_entries.size())) {
        LOGERR("CDBrowser::simpleAdd: bad obj index: " << m_popupidx
               << " id count: " << m_entries.size() << endl);
        return;
    }
    MetaDataList mdl;
    mdl.resize(1);
    udirentToMetadata(&m_entries[m_popupidx], &mdl[0]);
    emit sig_tracks_to_playlist(PlaylistAddMode(m_popupmode), false, mdl);
}

void CDBrowser::recursiveAdd(QAction *act)
{
    qDebug() << "CDBrowser::recursiveAdd";
    m_insertactive = true;
    m_popupmode = act->data().toInt();

    if (!m_ms) {
        qDebug() << "CDBrowser::browseContainer: server not set" ;
        return;
    }
    CDSH cds = m_ms->cds();
    if (!cds) {
        qDebug() << "Cant reach content directory service";
        return;
    }

    if (m_reaper) {
        delete m_reaper;
        m_reaper = 0;
    }
    
    m_recwalkentries.clear();
    m_reaper = new RecursiveReaper(cds, m_popupobjid, this);
    connect(m_reaper, 
            SIGNAL(sliceAvailable(const UPnPClient::UPnPDirContent *)),
            this, 
            SLOT(onReaperSliceAvailable(const UPnPClient::UPnPDirContent *)));
    connect(m_reaper, SIGNAL(done(int)), this, SLOT(rreaperDone(int)));
    m_reaper->start();
}

void CDBrowser::onReaperSliceAvailable(const UPnPClient::UPnPDirContent *dc)
{
    qDebug() << "CDBrowser::onReaperSliceAvailable: got " << 
        dc->m_items.size() << " items";
    for (unsigned int i = 0; i < dc->m_items.size(); i++) {
        m_recwalkentries.push_back(dc->m_items[i]);
    }
}

void CDBrowser::rreaperDone(int status)
{
    if (!m_reaper)
        return;
    qDebug() << "CDBrowser::rreaperDone: status: " << status;
    m_reaper->wait();
    delete m_reaper;
    m_reaper = 0;

    MetaDataList mdl;
    mdl.resize(m_recwalkentries.size());
    for (unsigned int i = 0; i <  m_recwalkentries.size(); i++) {
        udirentToMetadata(&m_recwalkentries[i], &mdl[i]);
    }
    emit sig_tracks_to_playlist(PlaylistAddMode(m_popupmode), false, mdl);
}
