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
#include "libupnpp/config.h"

#include <QUrl>
#ifdef USING_WEBENGINE
#warning acceptNavigationRequest is only called for http://. Change all links
// Notes for WebEngine
// - All links must be changed to http://
// - It seems that the links passed to acceptNav.. have the host part 
//   lowercased -> change S0 to http://s0, not http://S0

#include <QWebEnginePage>
#include <QWebEngineSettings>
#else
#include <QWebFrame>
#include <QWebSettings>
#include <QWebElement>
#include <QScriptEngine>
#endif

#include <QMenu>
#include <QApplication>
#include <QByteArray>

#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"

#include "libupnpp/log.hxx"
#include "libupnpp/control/discovery.hxx"

#include "upqo/cdirectory_qo.h"
#include "upadapt/upputils.h"
#include "upadapt/md5.hxx"
#include "cdbrowser.h"
#include "dirbrowser.h"
#include "rreaper.h"


using namespace std;
using namespace UPnPP;
using namespace UPnPClient;

static const string minimFoldersViewPrefix("0$folders");

// Escape things that would look like HTML markup
static string escapeHtml(const string &in)
{
    string out;
    for (string::size_type pos = 0; pos < in.length(); pos++) {
	switch(in.at(pos)) {
	case '<': out += "&lt;"; break;
	case '>': out += "&gt;"; break;
	case '&': out += "&amp;"; break;
	case '"': out += "&quot;"; break;
	default: out += in.at(pos); break;
	}
    }
    return out;
}

void CDWebPage::javaScriptConsoleMessage(
#ifdef USING_WEBENGINE
    JavaScriptConsoleMessageLevel,
#endif
    const QString& msg, int lineNum, const QString&)
{
    Q_UNUSED(msg);
    Q_UNUSED(lineNum);
    //qDebug() << "JAVASCRIPT: "<< msg << "at line " << lineNum;
}


CDBrowser::CDBrowser(QWidget* parent)
    : QWebView(parent), m_reader(0), m_reaper(0), m_browsers(0), 
      m_lastbutton(Qt::LeftButton)
{
    setPage(new CDWebPage(this));
#ifndef USING_WEBENGINE
    connect(this, SIGNAL(linkClicked(const QUrl &)), 
            this, SLOT(onLinkClicked(const QUrl &)));

    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
#endif

    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    if (parent) {
        settings()->setFontSize(QWebSettings::DefaultFontSize, 
                              parent->font().pointSize()+4);
	settings()->setFontFamily(QWebSettings::StandardFont, 
			       parent->font().family());
    }
    setStyleSheet(CSettingsStorage::getInstance()->getPlayerStyle());
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
	    this, SLOT(createPopupMenu(const QPoint&)));
#ifdef USING_WEBENGINE
#warning tobedone
#else
    connect(page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize&)),
            this, SLOT(onContentsSizeChanged(const QSize&)));
#endif

    setHtml("<html><head><title>Upplay directory browser !</title></head>"
            "<body><p>Looking for servers...</p>"
            "</body></html>");

    m_timer.setSingleShot(1);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(initialPage()));
    m_timer.start(0);
}

CDBrowser::~CDBrowser()
{
    deleteReaders();
}

void CDBrowser::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "CDBrowser::mouseReleaseEvent";
    m_lastbutton = event->button();
    QWebView::mouseReleaseEvent(event);
}

void CDBrowser::setStyleSheet(bool dark)
{
    QString cssfn = Helper::getSharePath() + "/cdbrowser/cdbrowser.css";
    QByteArray css = Helper::readFileToByteArray(cssfn);

    if (dark) {
        cssfn = Helper::getSharePath() + "/cdbrowser/dark.css";
        css +=  Helper::readFileToByteArray(cssfn);
    } else {
        cssfn = Helper::getSharePath() + "/cdbrowser/standard.css";
    }
    css +=  Helper::readFileToByteArray(cssfn);

    css = QByteArray("data:text/css;charset=utf-8;base64,") +
        css.toBase64();

    QUrl cssurl(QString::fromUtf8((const char*)css));
#ifdef USING_WEBENGINE
#warning tobedone
#else
    settings()->setUserStyleSheetUrl(cssurl);
#endif
}

void CDBrowser::onContentsSizeChanged(const QSize&)
{
    //qDebug() << "CDBrowser::onContentsSizeChanged: scrollpos " <<
    // page()->mainFrame()->scrollPosition();
#ifdef USING_WEBENGINE
    // No way to do this with webengine ?
#else
    page()->mainFrame()->setScrollPosition(m_savedscrollpos);
#endif
}

#ifdef USING_WEBENGINE
static QString base64_encode(QString string)
{
    QByteArray ba;
    ba.append(string);
    return ba.toBase64();
}
static void dumpHtml(QString html)
{
    qDebug() << "showHtml: " << html;
}

static void jsResult(QVariant res)
{
    qDebug() << "jsResult: " << res.toString();
}
#endif

void CDBrowser::appendHtml(const QString& elt_id, const QString& html)
{
    //qDebug()<<"CDBrowser::appendHtml: elt_id ["<<elt_id<< "] html "<< html;

#ifdef USING_WEBENGINE
    // With webengine, we can't access qt object from the page, so we
    // do everything in js. The data is encoded as base64 to avoid
    // quoting issues
    QString js;
    js = "var morehtml = \"" + base64_encode(html) + "\";\n";
    if (elt_id.isEmpty()) {
        js += QString("document.body.innerHTML += window.atob(morehtml);\n");
    } else {
        js += QString("document.getElementById(\"%1\").innerHTML += "
                     "window.atob(morehtml);\n").arg(elt_id);
    }
    qDebug() << "Executing JS: [" << js << "]";
    page()->runJavaScript(js, jsResult);
#else
    QWebFrame *mainframe = page()->mainFrame();
    StringObj morehtml(html);

    mainframe->addToJavaScriptWindowObject("morehtml", &morehtml, 
                                           SCRIPTOWNERSHIP);
    QString js;
    if (elt_id.isEmpty()) {
        js = QString("document.body.innerHTML += morehtml.text");
    } else {
        js = QString("document.getElementById(\"%1\").innerHTML += "
                     "morehtml.text").arg(elt_id);
    }
    mainframe->evaluateJavaScript(js);
#endif
}

void CDBrowser::onLinkClicked(const QUrl &url)
{
    m_timer.stop();
    string scurl = qs2utf8s(url.toString());
    //qDebug() << "CDBrowser::onLinkClicked: " << url.toString() << 
    //" button " <<  m_lastbutton << " mid " << Qt::MidButton;

    int what = scurl[0];

    switch (what) {

    case 'S':
    {
        // Servers page server link click: browse clicked server root
	unsigned int cdsidx = atoi(scurl.c_str()+1);
        if (cdsidx > m_msdescs.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad link index: " << cdsidx 
                   << " cd count: " << m_msdescs.size() << endl);
	    return;
	}
        m_curpath.clear();
        m_curpath.push_back(CtPathElt("0", "(servers)"));
        m_ms = MSRH(new MediaServer(m_msdescs[cdsidx]));
        if (!m_ms) {
            qDebug() << "MediaServer connect failed";
            return;
        }
        CDSH cds = m_ms->cds();
        if (cds) {
            cds->getSearchCapabilities(m_searchcaps);
            emit sig_searchcaps_changed();
        }
	browseContainer("0", m_msdescs[cdsidx].friendlyName);
    }
    break;

    case 'I':
    {
        // Directory listing item link clicked: add to playlist
	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_entries.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad objid index: " << i 
                   << " id count: " << m_entries.size() << endl);
	    return;
	}
        MetaDataList mdl;
        mdl.resize(1);
        udirentToMetadata(&m_entries[i], &mdl[0]);
        emit sig_tracks_to_playlist(mdl);
    }
    break;

    case 'C':
    {
        // Directory listing container link clicked: browse subdir.
#ifdef USING_WEBENGINE
#warning tobedone
#else
        m_curpath.back().scrollpos = page()->mainFrame()->scrollPosition();
#endif
	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_entries.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad objid index: " << i 
                   << " id count: " << m_entries.size() << endl);
	    return;
	}
        if (m_lastbutton == Qt::MidButton) {
            // Open in new tab
            vector<CtPathElt> npath(m_curpath);
            npath.push_back(CtPathElt(m_entries[i].m_id, m_entries[i].m_title));
            emit sig_browse_in_new_tab(u8s2qs(m_ms->desc()->UDN), npath);
        } else {
            browseContainer(m_entries[i].m_id, m_entries[i].m_title);
        }
        return;
    }
    break;

    case 'L':
    {
        // Click in curpath section.
	unsigned int i = atoi(scurl.c_str()+1);
        curpathClicked(i);
        return;
    }
    break;

    default:
        LOGERR("CDBrowser::onLinkClicked: bad link type: " << what << endl);
        return;
    }
}

void CDBrowser::curpathClicked(unsigned int i)
{
    //qDebug() << "CDBrowser::curpathClicked: " << i << " pathsize " << 
    // m_curpath.size();
    if (i >= m_curpath.size()) {
        LOGERR("CDBrowser::curPathClicked: bad curpath index: " << i 
               << " path count: " << m_curpath.size() << endl);
        return;
    }

    if (i == 0) {
        m_curpath.clear();
        m_msdescs.clear();
        m_ms = MSRH();
        initialPage();
    } else {
        string objid = m_curpath[i].objid;
        string title = m_curpath[i].title;
        string ss = m_curpath[i].searchStr;
        QPoint scrollpos = m_curpath[i].scrollpos;
        m_curpath.erase(m_curpath.begin() + i, m_curpath.end());
        if (m_lastbutton == Qt::MidButton) {
            vector<CtPathElt> npath(m_curpath);
            npath.push_back(CtPathElt(objid, title, ss));
            emit sig_browse_in_new_tab(u8s2qs(m_ms->desc()->UDN), npath);
        } else {
            if (ss.empty()) {
                browseContainer(objid, title, scrollpos);
            } else {
                search(objid, ss, scrollpos);
            }
        }
    }
}

void CDBrowser::browseIn(QString UDN, vector<CtPathElt> path)
{
    //qDebug() << "CDBrowser::browsein: " << UDN;

    m_curpath = path;

    if (m_msdescs.size() == 0) {
        // If the servers list is not ready yet, just set m_initUDN to tell
        // initialPage() to do the right thing later when it gets the servers
        // qDebug() << "CDBrowser::browsein: no servers";
        m_initUDN = UDN;
        return;
    } 

    m_lastbutton = Qt::LeftButton;

    // Find UDN in msdescs, create a MediaServer object, and read the
    // specified container (last in input curpath)
    string sudn = qs2utf8s(UDN);
    for (unsigned int i = 0; i < m_msdescs.size(); i++) {
        if (m_msdescs[i].UDN == sudn) {
            m_ms = MSRH(new MediaServer(m_msdescs[i]));
            curpathClicked(path.size() - 1);
            return;
        }
    }
    qDebug() << "CDBrowser::browsein: " << UDN << " not found";
}

static const QString init_container_pagetop(
    "<html><head>"
    "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"
    "<script type=\"text/javascript\">\n");

static const QString init_container_pagebot(
    "</script>\n"
#ifdef USING_WEBENGINE
    "</head><body onload=\"addEventListener('click', saveLoc)\">\n"
#else
    "</head><body>\n"
#endif
    "</body></html>"
    );

void CDBrowser::initContainerHtml(const string& ss)
{
    QString htmlpath("<div id=\"browsepath\"><ul>");
    bool current_is_search = false;
    for (unsigned i = 0; i < m_curpath.size(); i++) {
        QString title = QString::fromUtf8(m_curpath[i].title.c_str());
        QString objid = QString::fromUtf8(m_curpath[i].objid.c_str());
        QString sep("&gt;");
        if (!m_curpath[i].searchStr.empty()) {
            if (current_is_search) {
                // Indicate that searches are not nested by changing
                // the separator
                sep = "&lt;&gt;";
            }
            current_is_search = true;
        }
        if (i == 0)
            sep = "";
        htmlpath += QString("<li class=\"container\" objid=\"%3\">"
                            " %4 <a href=\"L%1\">%2</a></li>").
            arg(i).arg(title).arg(objid).arg(sep);
    }
    htmlpath += QString("</ul></div><br clear=\"all\"/>");
    if (!ss.empty()) {
        htmlpath += QString("Search results for: ") + 
            QString::fromUtf8(ss.c_str()) + "<br/>";
    }

    QString js;
#ifdef USING_WEBENGINE
    QString jsfn = Helper::getSharePath() + "/cdbrowser/containerscript.js";
    js = QString::fromUtf8(Helper::readFileToByteArray(jsfn));
#endif
    setHtml(init_container_pagetop + js + init_container_pagebot);
    waitForPage();
    appendHtml(QString(), htmlpath);
    appendHtml(QString(), "<table id=\"entrylist\"></table>");
}

// Re-browse (because sort criteria changed probably)
void CDBrowser::refresh()
{
    if (m_curpath.size() >= 1) {
        curpathClicked(m_curpath.size() - 1);
    }
}

void CDBrowser::browseContainer(string ctid, string cttitle, QPoint scrollpos)
{
    qDebug() << "CDBrowser::browseContainer: " << " ctid " << ctid.c_str();
    deleteReaders();

    emit sig_now_in(this, QString::fromUtf8(cttitle.c_str()));

    m_savedscrollpos = scrollpos;
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

    m_curpath.push_back(CtPathElt(ctid, cttitle));

    initContainerHtml();
    
    m_reader = new ContentDirectoryQO(cds, ctid, string(), this);

    connect(m_reader, SIGNAL(sliceAvailable(UPnPClient::UPnPDirContent*)),
            this, SLOT(onSliceAvailable(UPnPClient::UPnPDirContent *)));
    connect(m_reader, SIGNAL(done(int)), this, SLOT(onBrowseDone(int)));
    m_reader->start();
}

void CDBrowser::search(const string& iss)
{
    search(m_curpath.back().objid, iss);
}

void CDBrowser::search(const string& objid, const string& iss, QPoint scrollpos)
{
    deleteReaders();
    if (iss.empty())
        return;
    if (!m_ms) {
        LOGERR("CDBrowser::search: server not set" << endl);
        return;
    }
    CDSH cds = m_ms->cds();
    if (!cds) {
        LOGERR("Cant reach content directory service" << endl);
        return;
    }
    m_savedscrollpos = scrollpos;
    m_entries.clear();
    m_curpath.push_back(CtPathElt(objid, "(search)", iss));
    initContainerHtml(iss);

    m_reader = new ContentDirectoryQO(cds, m_curpath.back().objid, iss, this);

    connect(m_reader, SIGNAL(sliceAvailable(UPnPClient::UPnPDirContent*)),
            this, SLOT(onSliceAvailable(UPnPClient::UPnPDirContent *)));
    connect(m_reader, SIGNAL(done(int)), this, SLOT(onBrowseDone(int)));
    m_reader->start();
}


static QString CTToHtml(unsigned int idx, const UPnPDirObject& e)
{
    QString out;
    out += QString("<tr class=\"container\" objid=\"%1\" objidx=\"%2\">"
                   "<td></td><td>").arg(e.m_id.c_str());
    out += QString("<a class=\"ct_title\" href=\"C%1\">").arg(idx);
    out += QString::fromUtf8(escapeHtml(e.m_title).c_str());
    out += "</a></td>";
    string val;
    e.getprop("upnp:artist", val);
    QSettings settings;
    if (!val.empty() && settings.value("showartwithalb").toBool()) {
        int maxlen = settings.value("artwithalblen").toInt();
        if (maxlen && int(val.size()) > maxlen) {
            int len = maxlen-3 >= 0 ? maxlen-3 : 0;
            val = val.substr(0,len) + "...";
        }
        out += "<td class=\"ct_artist\">";
        out += QString::fromUtf8(escapeHtml(val).c_str());
        out += "</td>";
    }
    out += "</tr>";
    return out;
}

/** @arg idx index in entries array
    @arg e array entry
*/
static QString ItemToHtml(unsigned int idx, const UPnPDirObject& e,
                          int maxartlen)
{
    QString out;
    string val;

    out = QString("<tr class=\"item\" objid=\"%1\" objidx=\"%2\">").
        arg(e.m_id.c_str()).arg(idx);

    e.getprop("upnp:originalTrackNumber", val);
    out += QString("<td class=\"tk_tracknum\">") + 
        escapeHtml(val).c_str() + "</td>";

    out += "<td class=\"tk_title\">";
    out += QString("<a href=\"I%1\">").arg(idx);
    out += QString::fromUtf8(escapeHtml(e.m_title).c_str());
    out += "</a>";
    out += "</td>";

    val.clear();
    e.getprop("upnp:artist", val);
    out += "<td class=\"tk_artist\">";
    if (maxartlen > 0 && int(val.size()) > maxartlen) {
        int len = maxartlen-3 >= 0 ? maxartlen-3 : 0;
        val = val.substr(0,len) + "...";
    }
    out += QString::fromUtf8(escapeHtml(val).c_str());
    out += "</td>";
    
    val.clear();
    e.getprop("upnp:album", val);
    out += "<td class=\"tk_album\">";
    out += QString::fromUtf8(escapeHtml(val).c_str());
    out += "</td>";
    
    val.clear();
    e.getrprop(0, "duration", val);
    int seconds = upnpdurationtos(val);
    // Format as mm:ss
    int mins = seconds / 60;
    int secs = seconds % 60;
    char sdur[100];
    sprintf(sdur, "%02d:%02d", mins, secs);
    out += "<td class=\"tk_duration\">";
    out += sdur;
    out += "</td>";

    out += "</tr>";

    return out;
}

void CDBrowser::deleteReaders() 
{
    //qDebug() << "deleteReaders()";
    if (m_reader) {
        m_reader->setCancel();
        m_reader->wait();
        delete m_reader;
        m_reader = 0;
    }
    if (m_reaper) {
        m_reaper->setCancel();
        m_reaper->wait();
        delete m_reaper;
        m_reaper = 0;
    }
    // Give a chance to queued signals (a thread could have queued
    // something before/while we're cancelling it). Else, typically
    // onSliceAvailable could be called at a later time and mess the
    // display.
    qApp->processEvents();
}

void CDBrowser::onSliceAvailable(UPnPDirContent *dc)
{
    //qDebug() << "CDBrowser::onSliceAvailable";
    if (!m_reader) {
        qDebug() << "CDBrowser::onSliceAvailable: no reader";
        // Cancelled.
        delete dc;
        return;
    }
    QString html;

    m_entries.reserve(m_entries.size() + dc->m_containers.size() + 
                      dc->m_items.size());
    for (std::vector<UPnPDirObject>::iterator it = dc->m_containers.begin();
         it != dc->m_containers.end(); it++) {
        //qDebug() << "Container: " << it->dump().c_str();;
        m_entries.push_back(*it);
        html += CTToHtml(m_entries.size()-1, *it);
    }
    QSettings settings;
    int maxartlen = 0;
    if (settings.value("truncateartistindir").toBool()) {
        maxartlen = settings.value("truncateartistlen").toInt();
    }

    for (std::vector<UPnPDirObject>::iterator it = dc->m_items.begin();
         it != dc->m_items.end(); it++) {
        //qDebug() << "Item: " << it->dump().c_str();;
        m_entries.push_back(*it);
        html += ItemToHtml(m_entries.size()-1, *it, maxartlen);
    }
    appendHtml("entrylist", html);
    delete dc;
}

class DirObjCmp {
public:
    DirObjCmp(const vector<string>& crits)
        : m_crits(crits) {}
    bool operator()(const UPnPDirObject& o1, const UPnPDirObject& o2) {
        int rel;
        string s1, s2;
        for (unsigned int i = 0; i < m_crits.size(); i++) {
            const string& crit = m_crits[i];
            rel = dirobgetprop(o1, crit, s1).compare(
                dirobgetprop(o2, crit, s2));
            if (rel < 0)
                return true;
            else if (rel > 0)
                return false;
        }
        return false;
    }
    const string& dirobgetprop(const UPnPDirObject& o, const string& nm,
        string& storage) {
        if (!nm.compare("dc:title")) {
            return o.m_title;
        } else if (!nm.compare("uri")) {
            if (o.m_resources.size() == 0)
                return nullstr;
            return o.m_resources[0].m_uri;
        } 
        const string& prop = o.getprop(nm);
        if (!nm.compare("upnp:originalTrackNumber")) {
            char num[30];
            int i = atoi(prop.c_str());
            sprintf(num, "%010d", i);
            storage = num;
            return storage;
        } else {
            return prop;
        }
    }
    vector<string> m_crits;
    static string nullstr;
};
string DirObjCmp::nullstr;

void CDBrowser::onBrowseDone(int)
{
    //qDebug() <<"CDBrowser::onBrowseDone";
    if (!m_reader) {
        qDebug() << "CDBrowser::onBrowseDone(int) no reader: cancelled";
        return;
    }

    vector<string> sortcrits;
    int sortkind = CSettingsStorage::getInstance()->getSortKind();
    if (sortkind == CSettingsStorage::SK_MINIMFNORDER && 
        m_reader->getKind() == ContentDirectory::CDSKIND_MINIM && 
        m_curpath.back().searchStr.empty() &&
        m_reader->getObjid().compare(0, minimFoldersViewPrefix.size(),
                                     minimFoldersViewPrefix) == 0) {
        sortcrits.push_back("uri");
    } else if (sortkind == CSettingsStorage::SK_CUSTOM) {
        QStringList qcrits = CSettingsStorage::getInstance()->getSortCrits();
        for (int i = 0; i < qcrits.size(); i++) {
            sortcrits.push_back(qs2utf8s(qcrits[i]));
        }
    }

    if (!sortcrits.empty()) {
        DirObjCmp cmpo(sortcrits);

        sort(m_entries.begin(), m_entries.end(), cmpo);
        initContainerHtml();
        QString html;
        int maxartlen = 0;
        QSettings settings;
        if (settings.value("truncateartistindir").toBool()) {
            maxartlen = settings.value("truncateartistlen").toInt();
        }
        for (unsigned i = 0; i < m_entries.size(); i++) {
            if (m_entries[i].m_type == UPnPDirObject::container) {
                html += CTToHtml(i, m_entries[i]);
            } else {
                html += ItemToHtml(i, m_entries[i], maxartlen);
            }
        }
        appendHtml("entrylist", html);
    }

    m_reader->wait();
#ifdef USING_WEBENGINE
#warning tobedone
#else
    page()->mainFrame()->setScrollPosition(m_savedscrollpos);
#endif
    deleteReaders();

    //qDebug() << "CDBrowser::onBrowseDone done";
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
    out += QString("<p class=\"cdserver\" cdsid=\"%1\">").arg(idx);
    out += QString("<a href=\"S%1\">").arg(idx);
    out += QString::fromUtf8(dev.friendlyName.c_str());
    out += QString("</a></p>");
    return out;
}

void CDBrowser::waitForPage()
{
#ifdef USING_WEBENGINE
    QEventLoop loop;
    QTimer tT;
    tT.setSingleShot(true);
    connect(&tT, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(this, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()));
    tT.start(2000); 
    loop.exec();
#endif
}

void CDBrowser::initialPage()
{
    deleteReaders();
    emit sig_now_in(this, tr("Servers"));

    vector<UPnPDeviceDesc> msdescs;
    int secs = UPnPDeviceDirectory::getTheDir()->getRemainingDelay();
    if (secs > 1) {
        qDebug() << "CDBrowser::initialPage: waiting " << secs;
        m_timer.start(secs * 1000);
        return;
    }
    m_searchcaps.clear();
    emit sig_searchcaps_changed();
    if (!MediaServer::getDeviceDescs(msdescs)) {
        LOGERR("CDBrowser::initialPage: getDeviceDescs failed" << endl);
        return;
    }
    //qDebug() << "CDBrowser::initialPage: " << msdescs.size() << " servers";
    
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
        //LOGDEB("CDBrowser::initialPage: no change" << endl);
        m_timer.start(5000);
        return;
    } else {
        //qDebug() << "CDBrowser::initialPage: updating";
    }

    m_msdescs = msdescs;

    if (!m_initUDN.isEmpty()) {
        // "Browse in new tab": show appropriate container
        QString s = m_initUDN;
        m_initUDN = "";
        browseIn(s, m_curpath);
    } else {
        // Show servers list
        setHtml(QString::fromUtf8(init_server_page.c_str()));
        waitForPage();
        for (unsigned i = 0; i < msdescs.size(); i++) {
            appendHtml("", DSToHtml(i, msdescs[i]));
        }
        m_timer.start(5000);
    }
}

enum PopupMode {
    PUP_ADD,
    PUP_ADD_ALL,
    PUP_ADD_FROMHERE,
    PUP_BACK,
    PUP_OPEN_IN_NEW_TAB,
    PUP_RAND_PLAY_TRACKS,
    PUP_RAND_PLAY_ALBS,
    PUP_RAND_STOP,
};

void CDBrowser::createPopupMenu(const QPoint& pos)
{
    if (!m_browsers || m_browsers->insertActive())
        return;
    qDebug() << "void CDBrowser::createPopupMenu(const QPoint& pos)";

#ifdef USING_WEBENGINE
#warning tobedone
#else
    QWebHitTestResult htr = page()->mainFrame()->hitTestContent(pos);
    if (htr.isNull()) {
	return;
    }
    QWebElement el = htr.enclosingBlockElement();
    while (!el.isNull() && !el.hasAttribute("objid"))
	el = el.parent();

    QMenu *popup = new QMenu(this);
    QAction *act;
    QVariant v;
    if (m_curpath.size() != 0) {
        // Back action
        act = new QAction(tr("Back"), this);
        v = QVariant(int(PUP_BACK));
        act->setData(v);
        popup->addAction(act);
    }

    // Click in blank area: the only entry is Back
    if (el.isNull()) {
        popup->connect(popup, SIGNAL(triggered(QAction *)), this, 
                       SLOT(back(QAction *)));
        popup->popup(mapToGlobal(pos));
        return;
    }

    // Clicked on some object. Dir entries inside the path have no objidx attr
    // and that's ok
    m_popupobjid = qs2utf8s(el.attribute("objid"));
    m_popupobjtitle = qs2utf8s(el.toPlainText());
    if (el.hasAttribute("objidx"))
        m_popupidx = el.attribute("objidx").toInt();
    else
        m_popupidx = -1;

    QString otype = el.attribute("class");
    qDebug() << "Popup: " << " class " << otype << " objid " << 
        m_popupobjid.c_str();

    if (m_popupidx == -1 && otype.compare("container")) {
        // All path elements should be containers !
        qDebug() << "Not container and no objidx??";
        return;
    }

    act = new QAction(tr("Send to playlist"), this);
    v = QVariant(int(PUP_ADD));
    act->setData(v);
    popup->addAction(act);

    if (!otype.compare("item")) {
        act = new QAction(tr("Send all to playlist"), this);
        v = QVariant(int(PUP_ADD_ALL));
        act->setData(v);
        popup->addAction(act);

        act = new QAction(tr("Send all from here to playlist"), this);
        v = QVariant(int(PUP_ADD_FROMHERE));
        act->setData(v);
        popup->addAction(act);
    }
   
    // Connect to either recursive add or simpleAdd depending on entry type.
    if (!otype.compare("container")) {
        act = new QAction(tr("Open in new tab"), this);
        v = QVariant(int(PUP_OPEN_IN_NEW_TAB));
        act->setData(v);
        popup->addAction(act);

        act = new QAction(tr("Random play by tracks"), this);
        v = QVariant(int(PUP_RAND_PLAY_TRACKS));
        act->setData(v);
        popup->addAction(act);

        act = new QAction(tr("Random play by albums"), this);
        v = QVariant(int(PUP_RAND_PLAY_ALBS));
        act->setData(v);
        popup->addAction(act);

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

    // Like BACK, RAND_STOP must be processed by both simple and
    // recursiveAdd()
    act = new QAction(tr("Stop random play"), this);
    v = QVariant(int(PUP_RAND_STOP));
    act->setData(v);
    popup->addAction(act);
    act->setEnabled(m_browsers->randPlayActive());
    
    popup->popup(mapToGlobal(pos));
#endif
}

void CDBrowser::back(QAction *)
{
    if (m_curpath.size() >= 2)
        curpathClicked(m_curpath.size() - 2);
}

// Add a single track or a section of the current container. This
// maybe triggered by a link click or a popup on an item entry
void CDBrowser::simpleAdd(QAction *act)
{
    //qDebug() << "CDBrowser::simpleAdd";
    m_popupmode = act->data().toInt();
    if (m_popupmode == PUP_BACK) {
        back(0);
        return;
    }
    if (m_popupmode == PUP_RAND_STOP) {
        emit sig_rand_stop();
        return;
    }

    if (m_popupidx < 0 || m_popupidx > int(m_entries.size())) {
        LOGERR("CDBrowser::simpleAdd: bad obj index: " << m_popupidx
               << " id count: " << m_entries.size() << endl);
        return;
    }
    MetaDataList mdl;
    unsigned int starti = 0;
    switch (m_popupmode) {
    case PUP_ADD_FROMHERE: 
        starti = m_popupidx;
        /* FALLTHROUGH */
    case PUP_ADD_ALL:
        for (unsigned int i = starti; i < m_entries.size(); i++) {
            if (m_entries[i].m_type == UPnPDirObject::item && 
                m_entries[i].m_iclass == 
                UPnPDirObject::ITC_audioItem_musicTrack) {
                mdl.resize(mdl.size()+1);
                udirentToMetadata(&m_entries[i], &mdl[mdl.size()-1]);
            }
        }
        break;
    default:
        mdl.resize(1);
        udirentToMetadata(&m_entries[m_popupidx], &mdl[0]);
    }

    emit sig_tracks_to_playlist(mdl);
}

// Recursive add. This is triggered popup on a container
void CDBrowser::recursiveAdd(QAction *act)
{
    //qDebug() << "CDBrowser::recursiveAdd";
    m_popupmode = act->data().toInt();

    deleteReaders();

    if (m_popupmode == PUP_BACK) {
        back(0);
        return;
    }
    if (m_popupmode == PUP_RAND_STOP) {
        emit sig_rand_stop();
        return;
    }

    if (m_popupmode == PUP_OPEN_IN_NEW_TAB) {
        vector<CtPathElt> npath(m_curpath);
        npath.push_back(CtPathElt(m_popupobjid, m_popupobjtitle));
        emit sig_browse_in_new_tab(u8s2qs(m_ms->desc()->UDN), npath);
        return;
    }

    if (!m_ms) {
        qDebug() << "CDBrowser::recursiveAdd: server not set" ;
        return;
    }
    CDSH cds = m_ms->cds();
    if (!cds) {
        qDebug() << "Cant reach content directory service";
        return;
    }

    if (m_browsers)
        m_browsers->setInsertActive(true);
    ContentDirectory::ServiceKind kind = cds->getKind();
    if (kind == ContentDirectory::CDSKIND_MINIM) {
        // Use search() rather than a tree walk for Minim, it is much
        // more efficient.
        UPnPDirContent dirbuf;
        string ss("upnp:class = \"object.item.audioItem.musicTrack\"");
        int err = cds->search(m_popupobjid, ss, dirbuf);
        if (err != 0) {
            LOGERR("CDBrowser::recursiveAdd: search failed, code " << err);
            return;
        }

        m_recwalkentries = dirbuf.m_items;

        // If we are inside the folders view tree, and the option is
        // set, sort by url (minim sorts by tracknum tag even inside
        // folder view)
        if (CSettingsStorage::getInstance()->getSortKind() == 
            CSettingsStorage::SK_MINIMFNORDER && 
            m_popupobjid.compare(0, minimFoldersViewPrefix.size(),
                                 minimFoldersViewPrefix) == 0) {
            vector<string> crits;
            crits.push_back("uri");
            DirObjCmp cmpo(crits);
            sort(m_recwalkentries.begin(), m_recwalkentries.end(), cmpo);
        }

        rreaperDone(0);

    } else {
        m_recwalkentries.clear();
        m_recwalkdedup.clear();
        m_reaper = new RecursiveReaper(cds, m_popupobjid, this);
        connect(m_reaper, 
                SIGNAL(sliceAvailable(UPnPClient::UPnPDirContent *)),
                this, 
                SLOT(onReaperSliceAvailable(UPnPClient::UPnPDirContent *)));
        connect(m_reaper, SIGNAL(done(int)), this, SLOT(rreaperDone(int)));
        m_reaper->start();
    }
}

void CDBrowser::onReaperSliceAvailable(UPnPClient::UPnPDirContent *dc)
{
    qDebug() << "CDBrowser::onReaperSliceAvailable: got " << 
        dc->m_items.size() << " items";
    if (!m_reaper) {
        qDebug() << "CDBrowser::onReaperSliceAvailable: cancelled";
        delete dc;
        return;
    }
    for (unsigned int i = 0; i < dc->m_items.size(); i++) {
        if (dc->m_items[i].m_resources.empty()) {
            LOGDEB("CDBrowser::onReaperSlice: entry has no resources??"<< endl);
            continue;
        }
        //LOGDEB1("CDBrowser::onReaperSlice: uri: " << 
        //       dc->m_items[i].m_resources[0].m_uri << endl);
        string md5;
        MD5String(dc->m_items[i].m_resources[0].m_uri, md5);
        pair<STD_UNORDERED_SET<std::string>::iterator, bool> res = 
            m_recwalkdedup.insert(md5);
        if (res.second) {
            m_recwalkentries.push_back(dc->m_items[i]);
        } else {
            LOGDEB("CDBrowser::onReaperSlice: dup found" << endl);
        }
    }
    delete dc;
}

void CDBrowser::rreaperDone(int status)
{
    LOGDEB("CDBrowser::rreaperDone: status: " << status << ". Entries: " <<
           m_recwalkentries.size() << endl);
    deleteReaders();
    if (m_popupmode == PUP_RAND_PLAY_TRACKS ||
        m_popupmode == PUP_RAND_PLAY_ALBS) {
        // Sort list
        if (m_popupmode == PUP_RAND_PLAY_ALBS) {
            vector<string> sortcrits;
            sortcrits.push_back("upnp:album");
            sortcrits.push_back("upnp:originalTrackNumber");
            DirObjCmp cmpo(sortcrits);
            sort(m_recwalkentries.begin(), m_recwalkentries.end(), cmpo);
        }
        RandPlayer::PlayMode mode = m_popupmode == PUP_RAND_PLAY_TRACKS ?
            RandPlayer::PM_TRACKS : RandPlayer::PM_ALBS;
        LOGDEB("CDBrowser::rreaperDone: emitting sig_tracks_to_randplay, mode "
               << mode << endl);
        emit sig_tracks_to_randplay(mode, m_recwalkentries);
    } else {
        MetaDataList mdl;
        mdl.resize(m_recwalkentries.size());
        for (unsigned int i = 0; i <  m_recwalkentries.size(); i++) {
            udirentToMetadata(&m_recwalkentries[i], &mdl[i]);
        }
        emit sig_tracks_to_playlist(mdl);
    }
}
