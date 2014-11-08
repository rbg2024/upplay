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

#include <QToolButton>
#include <QTabBar>
#include <QIcon>
#include <QTimer>
#include <QShortcut>
#include <QLineEdit>

#include "dirbrowser.h"
#include "HelperStructs/Helper.h"
#include "upadapt/upputils.h"

DirBrowser::DirBrowser(QWidget *parent, Playlist *pl)
    : QWidget(parent), ui(new Ui::DirBrowser), m_pl(pl), m_insertactive(false)
{
    ui->setupUi(this);
    ui->searchFrame->hide();

    QToolButton *plusbut = new QToolButton(this);
    QString icon_path = Helper::getIconPath();
    QIcon icon = QIcon(icon_path + "/addtab.png");
    plusbut->setIcon(icon);
    ui->tabs->setCornerWidget(plusbut, Qt::TopRightCorner);
    plusbut->setCursor(Qt::ArrowCursor);
    plusbut->setAutoRaise(true);
    //QObject::connect(plusbut, SIGNAL(clicked()), this, SLOT(newTab()));
    plusbut->setToolTip(tr("Add page"));
    connect(plusbut, SIGNAL(clicked()), this, SLOT(addTab()));

    icon = QIcon(icon_path + "/cross.png");
    ui->closeSearchTB->setIcon(icon);
    connect(ui->closeSearchTB, SIGNAL(clicked()), this, SLOT(closeSearch()));

    QKeySequence seq("Ctrl+f");
    QShortcut *sc = new QShortcut(seq, this);
    connect(sc, SIGNAL (activated()), this, SLOT(openSearch()));
    seq = QKeySequence("/");
    sc = new QShortcut(seq, this);
    connect(sc, SIGNAL (activated()), this, SLOT(openSearch()));
    seq = QKeySequence("Esc");
    sc = new QShortcut(seq, this);
    connect(sc, SIGNAL (activated()), this, SLOT(closeSearch()));
    seq = QKeySequence("Ctrl+t");
    sc = new QShortcut(seq, this);
    connect(sc, SIGNAL (activated()), this, SLOT(addTab()));
    seq = QKeySequence("Ctrl+w");
    sc = new QShortcut(seq, this);
    connect(sc, SIGNAL (activated()), this, SLOT(closeCurrentTab()));

    seq = QKeySequence(Qt::Key_F3);
    sc = new QShortcut(seq, this);
    connect(sc, SIGNAL(activated()), this, SLOT(searchNext()));
    seq = QKeySequence(Qt::SHIFT + Qt::Key_F3);
    sc = new QShortcut(seq, this);
    connect(sc, SIGNAL(activated()), this, SLOT(searchPrev()));
    connect(ui->searchCMB->lineEdit(), SIGNAL(textChanged(const QString&)), 
            this, SLOT(onTextChanged(const QString&)));
    connect(ui->nextTB, SIGNAL(clicked()), this, SLOT(searchNext()));
    connect(ui->prevTB, SIGNAL(clicked()), this, SLOT(searchPrev()));
    connect(ui->searchCMB->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(returnPressedInSearch()));

    connect(ui->tabs, SIGNAL(currentChanged(int)), 
            this, SLOT(onCurrentChanged(int)));
    connect(m_pl, SIGNAL(insertDone()), this, SLOT(onInsertDone()));
   
    connect(ui->serverSearchCB, SIGNAL(stateChanged(int)), 
            this, SLOT(onSearchKindChanged(int)));
    onSearchKindChanged(int(ui->serverSearchCB->checkState()));

    setupTabConnections(0);
    setPlaylist(pl);
}

void DirBrowser::onSearchKindChanged(int state)
{
    if (state == Qt::Unchecked) {
        ui->propsCMB->setEnabled(false);
        ui->nextTB->setEnabled(true);
        ui->prevTB->setEnabled(true);
    } else {
        ui->propsCMB->setEnabled(true);
        ui->nextTB->setEnabled(false);
        ui->prevTB->setEnabled(false);
    }
}

void DirBrowser::setPlaylist(Playlist *pl)
{
    m_pl = pl;

    for (int i = 0; i < ui->tabs->count(); i++) {
        setupTabConnections(i);
    }
}

void DirBrowser::setInsertActive(bool onoff)
{
    m_insertactive = onoff;
}

bool DirBrowser::insertActive()
{
    return m_insertactive;
}

// Any of the tabs searchcaps changed. Update the current one just in case.
void DirBrowser::on_searchcaps_changed()
{
    onCurrentChanged(-1);
}

void DirBrowser::onCurrentChanged(int )
{
    CDBrowser *cdb = currentBrowser();
    set<string> caps;
    if (cdb)
        cdb->getSearchCaps(caps);
    while(ui->propsCMB->count())
        ui->propsCMB->removeItem(0);
    if (caps.empty()) {
        ui->serverSearchCB->setCheckState(Qt::Unchecked);
        ui->serverSearchCB->setEnabled(false);
    } else {
        ui->serverSearchCB->setEnabled(true);
        qDebug() << "Search Caps: ";
        vector<pair<string, string> > props;
        props.push_back(pair<string,string>("upnp:artist", "Artist"));
        props.push_back(pair<string,string>("upnp:album", "Album"));
        props.push_back(pair<string,string>("dc:title", "Title"));
        props.push_back(pair<string,string>("upnp:genre", "Genre"));
        for (auto it = props.begin(); it != props.end(); it++) {
            if (caps.find("*") != caps.end() || 
                caps.find(it->first) != caps.end()) {
                ui->propsCMB->addItem(u8s2qs(it->second),
                                      QVariant(u8s2qs(it->first)));
                qDebug() << it->second.c_str();
            }
        }
        onSearchKindChanged(ui->serverSearchCB->checkState());
    }
}

void DirBrowser::serverSearch()
{
    QString text = ui->searchCMB->lineEdit()->text();
    int i = ui->propsCMB->currentIndex();
    string prop = qs2utf8s(ui->propsCMB->itemData(i).toString());
    if (text != "") {
        string iss = qs2utf8s(text);
        string ss(prop);
        ss += " contains \"";
        for (unsigned i = 0; i < iss.size(); i++) {
            if (iss[i] == '"' || iss[i] == '\\')
                ss += string("\\");
            ss += iss[i];
        }
        ss += '"';
        CDBrowser *cdb = currentBrowser();
        if (cdb)
            cdb->search(ss);
    }
}

void DirBrowser::openSearch()
{
    onCurrentChanged(-1);
    ui->searchFrame->show();
    ui->searchCMB->lineEdit()->setFocus();
    ui->searchCMB->lineEdit()->selectAll();
}

void DirBrowser::returnPressedInSearch()
{
    if (ui->propsCMB->isEnabled()) {
        serverSearch();
    } else {
        QString text = ui->searchCMB->lineEdit()->text();
        if (ui->searchCMB->findText(text))
            ui->searchCMB->insertItem(0, text);
        searchNext();
    }
}

void DirBrowser::closeSearch()
{
    QString text = ui->searchCMB->lineEdit()->text();
    if (ui->searchCMB->findText(text))
        ui->searchCMB->insertItem(0, text);
    ui->searchFrame->hide();
}

// Perform text search in current tab. 
// @param next if true, we look for the next match of the
// current search, trying to advance and possibly wrapping around. Else is
// false, the search string has been modified, we search for the new string, 
// starting from the current position
void DirBrowser::doSearch(const QString& text, bool reverse)
{
    qDebug() << "DirBrowser::doSearch: text " << text << " reverse " << reverse;

    CDBrowser *cdb = currentBrowser();
    if (cdb == 0) {
	return;
    }
    QWebPage::FindFlags options = QWebPage::FindWrapsAroundDocument;
    if (reverse)
        options |= QWebPage::FindBackward;

    cdb->findText(text, options);
}

void DirBrowser::searchNext()
{
    doSearch(ui->searchCMB->currentText(), false);
}

void DirBrowser::searchPrev()
{
    doSearch(ui->searchCMB->currentText(), true);
}

void DirBrowser::onTextChanged(const QString& text)
{
    if (!ui->propsCMB->isEnabled()) {
        qDebug() << "Search line text changed: " << text;
        if (!text.isEmpty()) {
            doSearch(text, false);
        }
    }
}

CDBrowser *DirBrowser::currentBrowser()
{
    QWidget *tw = ui->tabs->currentWidget();
    if (tw == 0)
        return 0;
    return tw->findChild<CDBrowser*>();
}

void DirBrowser::setupTabConnections(int i)
{
    QWidget *tw = ui->tabs->widget(i);
    CDBrowser *cdb;
    if (tw) {
        cdb = tw->findChild<CDBrowser*>();
    } else {
        qDebug() << "Tab " << i << " not found";
    }
    if (!cdb) {
        qDebug() << "Tab " << i << " has no browser child";
        return;
    }
    setupTabConnections(cdb);
}

void DirBrowser::setupTabConnections(CDBrowser *cdb)
{
    cdb->setDirBrowser(this);
    disconnect(cdb, SIGNAL(sig_now_in(QWidget *, const QString&)), 0, 0);
    connect(cdb, SIGNAL(sig_now_in(QWidget *, const QString&)), 
            this, SLOT(onTabTitleNeedChange(QWidget *, const QString&)));

    connect(cdb, SIGNAL(sig_tracks_to_playlist(const MetaDataList&)),
            m_pl, SLOT(psl_add_tracks(const MetaDataList&)));
    connect(cdb, SIGNAL(sig_searchcaps_changed()), 
            this, SLOT(on_searchcaps_changed()));
}

void DirBrowser::setStyleSheet(bool dark)
{
    for (int i = 0; i < ui->tabs->count(); i++) {
        QWidget *tw = ui->tabs->widget(i);
        if (tw) {
	    CDBrowser *cdb = tw->findChild<CDBrowser*>();
            if (cdb) {
		cdb->setStyleSheet(dark);
            }
        }
    }
}

void DirBrowser::closeCurrentTab()
{
    if (m_insertactive)
        return;
    QWidget *w = ui->tabs->currentWidget();
    ui->tabs->removeTab(ui->tabs->currentIndex());
    delete w;
}

void DirBrowser::addTab()
{
    QWidget *tab = new QWidget(this);
    tab->setObjectName(QString::fromUtf8("tab"));
    QVBoxLayout* vl = new QVBoxLayout(tab);

    CDBrowser *cdb = new CDBrowser(tab);
    cdb->setObjectName(QString::fromUtf8("cdBrowser"));
    vl->addWidget(cdb);

    ui->tabs->addTab(tab, tr("Servers"));

    setupTabConnections(cdb);
}

void DirBrowser::onTabTitleNeedChange(QWidget *w, const QString& tt)
{
    int i = ui->tabs->indexOf((QWidget*)w->parent());
    if (i >= 0) {
        ui->tabs->setTabText(i, tt);
    } else {
        qDebug() << "Widget not found in tabs: " << w;
    }
}
