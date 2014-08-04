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

#include "cdbrowser.h"


CDBrowser::CDBrowser(QWidget* parent)
    : QWebView(parent)
{
    connect(this, SIGNAL(linkClicked(const QUrl &)), 
	    this, SLOT(onLinkClicked(const QUrl &)));
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    setHtml("<html><head><title>Hello World !</title></head>"
            "<body><p>Hello World !</p>"
            "<a href=\"linktarget\">This is a link</a>"
            "</body></html>");
}

CDBrowser::~CDBrowser()
{

}

void CDBrowser::appendHtml(const QString& html)
{
    cerr << "CDBrowser::appendHtml: " << 
        (const char *)html.toLatin1() << endl;

    QWebFrame *mainframe = page()->mainFrame();
    StringObj morehtml(html);

    mainframe->addToJavaScriptWindowObject("morehtml", &morehtml, 
                                           QScriptEngine::ScriptOwnership);
    QString js("document.body.innerHTML += morehtml.text");
    mainframe->evaluateJavaScript(js);
}

void CDBrowser::onLinkClicked(const QUrl &url)
{
    cerr << "CDBrowser::onLinkClicked: " << (const char *)url.toString().toUtf8() << endl;
}

