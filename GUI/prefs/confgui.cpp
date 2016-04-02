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

#include "confgui.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include <qglobal.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QFrame>
#include <QListWidget>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <qobject.h>
#include <qlayout.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qtooltip.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qinputdialog.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qcombobox.h>

using namespace std;

namespace confgui {

static const int spacing = 3;
static const int margin = 3;

ConfTabsW::ConfTabsW(QWidget *parent, const QString& title, 
                     ConfLinkFact *fact)
    : QDialog(parent), m_makelink(fact)
{
    setWindowTitle(title);
    tabWidget = new QTabWidget;

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
				     | QDialogButtonBox::Cancel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    resize(QSize(500, 400).expandedTo(minimumSizeHint()));

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptChanges()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(rejectChanges()));
}

void ConfTabsW::acceptChanges()
{
    cerr << "ConfTabsW::acceptChanges()\n";
    for (vector<ConfPanelW *>::iterator it = m_panels.begin(); 
         it != m_panels.end(); it++) {
        (*it)->storeValues();
    }
    for (vector<ConfPanelWIF *>::iterator it = m_widgets.begin(); 
         it != m_widgets.end(); it++) {
        (*it)->storeValues();
    }
    emit sig_prefsChanged();
    hide();
}

void ConfTabsW::rejectChanges()
{
    cerr << "ConfTabsW::rejectChanges()\n";
    reloadPanels();
    hide();
}

void ConfTabsW::reloadPanels()
{
    for (vector<ConfPanelW *>::iterator it = m_panels.begin(); 
         it != m_panels.end(); it++) {
        (*it)->loadValues();
    }
    for (vector<ConfPanelWIF *>::iterator it = m_widgets.begin(); 
         it != m_widgets.end(); it++) {
        (*it)->loadValues();
    }
}

int ConfTabsW::addPanel(const QString& title)
{
    ConfPanelW *w = new ConfPanelW(this);
    m_panels.push_back(w);
    return tabWidget->addTab(w, title);
}

int ConfTabsW::addForeignPanel(ConfPanelWIF* w, const QString& title)
{
    m_widgets.push_back(w);
    QWidget *qw = dynamic_cast<QWidget *>(w);
    if (qw == 0) {
        qDebug() << "Can't cast panel to QWidget";
        abort();
    }
    return tabWidget->addTab(qw, title);
}

ConfParamW *ConfTabsW::addParam(int tabindex,
                             ParamType tp, const QString& varname,
                             const QString& label, const QString& tooltip, 
                             int ival, int maxval, 
                             const QStringList* sl)
{
    ConfLink lnk = (*m_makelink)(varname);

    ConfPanelW *panel = (ConfPanelW*)tabWidget->widget(tabindex);
    if (panel == 0) 
        return 0;

    ConfParamW *cp = 0;
    switch (tp) {
    case CFPT_BOOL:
        cp = new ConfParamBoolW(this, lnk, label, tooltip);
        break;
    case CFPT_INT: 
	cp = new ConfParamIntW(this, lnk, label, tooltip, ival, maxval, 
                               ival);
        break;
    case CFPT_STR:
	cp = new ConfParamStrW(this, lnk, label, tooltip);
        break;
    case CFPT_CSTR: 
	cp = new ConfParamCStrW(this, lnk, label, tooltip, *sl);
        break;
    case CFPT_FN:
	cp = new ConfParamFNW(this, lnk, label, tooltip, ival);
        break;
    case CFPT_STRL:
	cp = new ConfParamSLW(this, lnk, label, tooltip);
    case CFPT_DNL: 
	cp = new ConfParamDNLW(this, lnk, label, tooltip);
        break;
    case CFPT_CSTRL:
	cp = new ConfParamCSLW(this, lnk, label, tooltip, *sl);
        break;

    }
    panel->addWidget(cp);
    m_params.push_back(cp);
    return cp;
}

bool ConfTabsW::enableLink(ConfParamW* boolw, ConfParamW* otherw, bool revert)
{
    if (std::find(m_params.begin(), m_params.end(), boolw) == m_params.end() ||
        std::find(m_params.begin(), m_params.end(), otherw) == m_params.end()) {
        cerr << "ConfTabsW::enableLink: param not found\n";
        return false;
    }
    ConfParamBoolW *bw = dynamic_cast<ConfParamBoolW*>(boolw);
    if (bw == 0) {
        cerr << "ConfTabsW::enableLink: not a boolw\n";
        return false;
    }
    otherw->setEnabled(revert?!bw->m_cb->isChecked():bw->m_cb->isChecked());
    if (revert) {
        connect(bw->m_cb, SIGNAL(toggled(bool)),
                otherw, SLOT(setDisabled(bool)));
    } else {
        connect(bw->m_cb, SIGNAL(toggled(bool)),
                otherw, SLOT(setEnabled(bool)));
    }
    return true;
}

ConfPanelW::ConfPanelW(QWidget *parent)
    : QWidget(parent)
{
    m_vboxlayout = new QVBoxLayout(this);
    m_vboxlayout->setSpacing(spacing);
    m_vboxlayout->setMargin(margin);
    m_vboxlayout->insertStretch(-1);
}

void ConfPanelW::addWidget (QWidget *w) 
{
    m_vboxlayout->addWidget(w);
    m_widgets.push_back(w);
}

void ConfPanelW::storeValues()
{
    for (vector<QWidget *>::iterator it = m_widgets.begin(); 
         it != m_widgets.end(); it++) {
        ConfParamW *p = (ConfParamW*)*it;
        p->storeValue();
    }
}

void ConfPanelW::loadValues()
{
    for (vector<QWidget *>::iterator it = m_widgets.begin(); 
         it != m_widgets.end(); it++) {
        ConfParamW *p = (ConfParamW*)*it;
        p->loadValue();
    }
}

static bool stringToBool(const string &s)
{
    if (s.empty())
	return false;
    if (isdigit(s[0])) {
	int val = atoi(s.c_str());
	return val ? true : false;
    }
    if (s.find_first_of("yYtT") == 0)
	return true;
    return false;
}

template <class T> bool stringToStrings(const string &s, T &tokens, 
                                        const string& addseps)
{
    string current;
    tokens.clear();
    enum states {SPACE, TOKEN, INQUOTE, ESCAPE};
    states state = SPACE;
    for (unsigned int i = 0; i < s.length(); i++) {
	switch (s[i]) {
        case '"': 
	    switch(state) {
            case SPACE: 
		state=INQUOTE; continue;
            case TOKEN: 
	        current += '"';
		continue;
            case INQUOTE: 
                tokens.insert(tokens.end(), current);
		current.clear();
		state = SPACE;
		continue;
            case ESCAPE:
	        current += '"';
	        state = INQUOTE;
                continue;
	    }
	    break;
        case '\\': 
	    switch(state) {
            case SPACE: 
            case TOKEN: 
                current += '\\';
                state=TOKEN; 
                continue;
            case INQUOTE: 
                state = ESCAPE;
                continue;
            case ESCAPE:
                current += '\\';
                state = INQUOTE;
                continue;
	    }
	    break;

        case ' ': 
        case '\t': 
        case '\n': 
        case '\r': 
	    switch(state) {
            case SPACE: 
                continue;
            case TOKEN: 
		tokens.insert(tokens.end(), current);
		current.clear();
		state = SPACE;
		continue;
            case INQUOTE: 
            case ESCAPE:
                current += s[i];
                continue;
	    }
	    break;

        default:
            if (!addseps.empty() && addseps.find(s[i]) != string::npos) {
                switch(state) {
                case ESCAPE:
                    state = INQUOTE;
                    break;
                case INQUOTE: 
                    break;
                case SPACE: 
                    tokens.insert(tokens.end(), string(1, s[i]));
                    continue;
                case TOKEN: 
                    tokens.insert(tokens.end(), current);
                    current.erase();
                    tokens.insert(tokens.end(), string(1, s[i]));
                    state = SPACE;
                    continue;
                }
            } else switch(state) {
                case ESCAPE:
                    state = INQUOTE;
                    break;
                case SPACE: 
                    state = TOKEN;
                    break;
                case TOKEN: 
                case INQUOTE: 
                    break;
                }
	    current += s[i];
	}
    }
    switch(state) {
    case SPACE: 
	break;
    case TOKEN: 
	tokens.insert(tokens.end(), current);
	break;
    case INQUOTE: 
    case ESCAPE:
	return false;
    }
    return true;
}

template bool stringToStrings<vector<string> >(const string &, 
					       vector<string> &, const string&);
template <class T> void stringsToString(const T &tokens, string &s) 
{
    for (typename T::const_iterator it = tokens.begin();
	 it != tokens.end(); it++) {
	bool hasblanks = false;
	if (it->find_first_of(" \t\n") != string::npos)
	    hasblanks = true;
	if (it != tokens.begin())
	    s.append(1, ' ');
	if (hasblanks)
	    s.append(1, '"');
	for (unsigned int i = 0; i < it->length(); i++) {
	    char car = it->at(i);
	    if (car == '"') {
		s.append(1, '\\');
		s.append(1, car);
	    } else {
		s.append(1, car);
	    }
	}
	if (hasblanks)
	    s.append(1, '"');
    }
}
template void stringsToString<vector<string> >(const vector<string> &,string &);
template <class T> string stringsToString(const T &tokens)
{
    string out;
    stringsToString<T>(tokens, out);
    return out;
}
template string stringsToString<vector<string> >(const vector<string> &);

static QString myGetFileName(bool isdir, QString caption = QString(),
			     bool filenosave = false);

static QString myGetFileName(bool isdir, QString caption, bool filenosave)
{
    QFileDialog dialog(0, caption);

    if (isdir) {
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOptions(QFileDialog::ShowDirsOnly);
    } else {
	dialog.setFileMode(QFileDialog::AnyFile);
	if (filenosave)
	    dialog.setAcceptMode(QFileDialog::AcceptOpen);
	else
	    dialog.setAcceptMode(QFileDialog::AcceptSave);
    }
    dialog.setViewMode(QFileDialog::List);
    QFlags<QDir::Filter> flags = QDir::NoDotAndDotDot | QDir::Hidden; 
    if (isdir)
	flags |= QDir::Dirs;
    else 
	flags |= QDir::Dirs | QDir::Files;
    dialog.setFilter(flags);

    if (dialog.exec() == QDialog::Accepted) {
        return dialog.selectedFiles().value(0);
    }
    return QString();
}

void ConfParamW::setValue(const QString& value)
{
    if (m_fsencoding)
        m_cflink->set(string((const char *)value.toLocal8Bit()));
    else
        m_cflink->set(string((const char *)value.toUtf8()));
}

void ConfParamW::setValue(int value)
{
    char buf[30];
    sprintf(buf, "%d", value);
    m_cflink->set(string(buf));
}

void ConfParamW::setValue(bool value)
{
    char buf[30];
    sprintf(buf, "%d", value);
    m_cflink->set(string(buf));
}

extern void setSzPol(QWidget *w, QSizePolicy::Policy hpol, 
                     QSizePolicy::Policy vpol,
                     int hstretch, int vstretch);

void setSzPol(QWidget *w, QSizePolicy::Policy hpol, 
		   QSizePolicy::Policy vpol,
		   int hstretch, int vstretch)
{
    QSizePolicy policy(hpol, vpol);
    policy.setHorizontalStretch(hstretch);
    policy.setVerticalStretch(vstretch);
    policy.setHeightForWidth(w->sizePolicy().hasHeightForWidth());
    w->setSizePolicy(policy);
}

bool ConfParamW::createCommon(const QString& lbltxt, const QString& tltptxt)
{
    m_hl = new QHBoxLayout(this);
    m_hl->setSpacing(spacing);

    QLabel *tl = new QLabel(this);
    setSzPol(tl, QSizePolicy::Preferred, QSizePolicy::Fixed, 0, 0);
    tl->setText(lbltxt);
    tl->setToolTip(tltptxt);

    m_hl->addWidget(tl);

    return true;
}

ConfParamIntW::ConfParamIntW(QWidget *parent, ConfLink cflink, 
			     const QString& lbltxt,
			     const QString& tltptxt,
			     int minvalue, 
			     int maxvalue,
                             int defaultvalue)
    : ConfParamW(parent, cflink), m_defaultvalue(defaultvalue)
{
    if (!createCommon(lbltxt, tltptxt))
	return;

    m_sb = new QSpinBox(this);
    m_sb->setMinimum(minvalue);
    m_sb->setMaximum(maxvalue);
    setSzPol(m_sb, QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0);
    m_hl->addWidget(m_sb);

    QFrame *fr = new QFrame(this);
    setSzPol(fr, QSizePolicy::Preferred, QSizePolicy::Fixed, 0, 0);
    m_hl->addWidget(fr);

    loadValue();
}

void ConfParamIntW::storeValue()
{
    setValue(m_sb->value());
}

void ConfParamIntW::loadValue()
{
    string s;
    if (m_cflink->get(s)) 
        m_sb->setValue(atoi(s.c_str()));
    else
        m_sb->setValue(m_defaultvalue);
}

ConfParamStrW::ConfParamStrW(QWidget *parent, ConfLink cflink, 
			     const QString& lbltxt,
			     const QString& tltptxt)
    : ConfParamW(parent, cflink)
{
    if (!createCommon(lbltxt, tltptxt))
	return;

    m_le = new QLineEdit(this);
    setSzPol(m_le, QSizePolicy::Preferred, QSizePolicy::Fixed, 1, 0);

    m_hl->addWidget(m_le);

    loadValue();
}

void ConfParamStrW::storeValue()
{
    setValue(m_le->text());
}

void ConfParamStrW::loadValue()
{
    string s;
    m_cflink->get(s);
    if (m_fsencoding)
        m_le->setText(QString::fromLocal8Bit(s.c_str()));
    else
        m_le->setText(QString::fromUtf8(s.c_str()));
}

ConfParamCStrW::ConfParamCStrW(QWidget *parent, ConfLink cflink, 
			       const QString& lbltxt,
			       const QString& tltptxt,
			       const QStringList &sl
			       )
    : ConfParamW(parent, cflink)
{
    if (!createCommon(lbltxt, tltptxt))
	return;
    m_cmb = new QComboBox(this);
    m_cmb->setEditable(false);
    m_cmb->insertItems(0, sl);

    setSzPol(m_cmb, QSizePolicy::Preferred, QSizePolicy::Fixed, 1, 0);

    m_hl->addWidget(m_cmb);

    loadValue();
}

void ConfParamCStrW::storeValue()
{
    setValue(m_cmb->currentText());
}

void ConfParamCStrW::loadValue()
{
    string s;
    m_cflink->get(s);
    QString cs;
    if (m_fsencoding)
        cs = QString::fromLocal8Bit(s.c_str());
    else
        cs = QString::fromUtf8(s.c_str());
        
    for (int i = 0; i < m_cmb->count(); i++) {
	if (!cs.compare(m_cmb->itemText(i))) {
	    m_cmb->setCurrentIndex(i);
	    break;
	}
    }
}

ConfParamBoolW::ConfParamBoolW(QWidget *parent, ConfLink cflink, 
			       const QString& lbltxt,
			       const QString& tltptxt)
    : ConfParamW(parent, cflink)
{
    // No createCommon because the checkbox has a label
    m_hl = new QHBoxLayout(this);
    m_hl->setSpacing(spacing);

    m_cb = new QCheckBox(lbltxt, this);
    setSzPol(m_cb, QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0);
    m_cb->setToolTip(tltptxt);
    m_hl->addWidget(m_cb);

    QFrame *fr = new QFrame(this);
    setSzPol(fr, QSizePolicy::Preferred, QSizePolicy::Fixed, 1, 0);
    m_hl->addWidget(fr);

    loadValue();
}

void ConfParamBoolW::storeValue()
{
    setValue(m_cb->isChecked());
}

void ConfParamBoolW::loadValue()
{
    string s;
    m_cflink->get(s);
    m_cb->setChecked(stringToBool(s));
}

ConfParamFNW::ConfParamFNW(QWidget *parent, ConfLink cflink, 
			   const QString& lbltxt,
			   const QString& tltptxt,
			   bool isdir
			   )
    : ConfParamW(parent, cflink), m_isdir(isdir)
{
    if (!createCommon(lbltxt, tltptxt))
	return;

    m_fsencoding = true;

    m_le = new QLineEdit(this);
    m_le->setMinimumSize(QSize(150, 0 ));
    setSzPol(m_le, QSizePolicy::Preferred, QSizePolicy::Fixed, 1, 0);
    m_hl->addWidget(m_le);

    m_pb = new QPushButton(this);
    
    QString text = tr("Choose");
    m_pb->setText(text);
    int width = m_pb->fontMetrics().boundingRect(text).width() + 15;
    m_pb->setMaximumWidth(width);
    setSzPol(m_pb, QSizePolicy::Minimum, QSizePolicy::Fixed, 0, 0);
    m_hl->addWidget(m_pb);

    loadValue();
    QObject::connect(m_pb, SIGNAL(clicked()), this, SLOT(showBrowserDialog()));
}

void ConfParamFNW::storeValue()
{
    setValue(m_le->text());
}

void ConfParamFNW::loadValue()
{
    string s;
    m_cflink->get(s);
    m_le->setText(QString::fromLocal8Bit(s.c_str()));
}

void ConfParamFNW::showBrowserDialog()
{
    QString s = myGetFileName(m_isdir);
    if (!s.isEmpty())
        m_le->setText(s);
}

class SmallerListWidget: public QListWidget 
{
public:
    SmallerListWidget(QWidget *parent)
	: QListWidget(parent) {}
    virtual QSize sizeHint() const {return QSize(150, 40);}
};

ConfParamSLW::ConfParamSLW(QWidget *parent, ConfLink cflink, 
			   const QString& lbltxt,
			   const QString& tltptxt)
    : ConfParamW(parent, cflink)
{
    // Can't use createCommon here cause we want the buttons below the label
    m_hl = new QHBoxLayout(this);
    m_hl->setSpacing(spacing);

    QVBoxLayout *vl1 = new QVBoxLayout();
    QHBoxLayout *hl1 = new QHBoxLayout();

    QLabel *tl = new QLabel(this);
    setSzPol(tl, QSizePolicy::Preferred, QSizePolicy::Fixed, 0, 0);
    tl->setText(lbltxt);
    tl->setToolTip(tltptxt);
    vl1->addWidget(tl);

    QPushButton *pbA = new QPushButton(this);
    QString text = tr("+");
    pbA->setText(text);
    int width = pbA->fontMetrics().boundingRect(text).width() + 15;
    pbA->setMaximumWidth(width);
    setSzPol(pbA, QSizePolicy::Minimum, QSizePolicy::Fixed, 0, 0);
    hl1->addWidget(pbA);

    QPushButton *pbD = new QPushButton(this);
    text = tr("-");
    pbD->setText(text);
    width = pbD->fontMetrics().boundingRect(text).width() + 15;
    pbD->setMaximumWidth(width);
    setSzPol(pbD, QSizePolicy::Minimum, QSizePolicy::Fixed, 0, 0);
    hl1->addWidget(pbD);

    vl1->addLayout(hl1);
    m_hl->addLayout(vl1);

    m_lb = new SmallerListWidget(this);
    m_lb->setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSzPol(m_lb, QSizePolicy::Preferred, QSizePolicy::Preferred, 1, 1);
    m_hl->addWidget(m_lb);

    setSzPol(this, QSizePolicy::Preferred, QSizePolicy::Preferred, 1, 1);
    loadValue();
    QObject::connect(pbA, SIGNAL(clicked()), this, SLOT(showInputDialog()));
    QObject::connect(pbD, SIGNAL(clicked()), this, SLOT(deleteSelected()));
}

void ConfParamSLW::storeValue()
{
    vector<string> ls;
    for (int i = 0; i < m_lb->count(); i++) {
        // General parameters are encoded as utf-8. File names as
        // local8bit There is no hope for 8bit file names anyway
        // except for luck: the original encoding is unknown.
	QString text = m_lb->item(i)->text();
        if (m_fsencoding)
            ls.push_back((const char *)(text.toLocal8Bit()));
        else
            ls.push_back((const char *)(text.toUtf8()));
    }
    string s;
    stringsToString(ls, s);
    m_cflink->set(s);
}

void ConfParamSLW::loadValue()
{
    string s;
    m_cflink->get(s);
    vector<string> ls; 
    stringToStrings(s, ls);
    QStringList qls;
    for (vector<string>::const_iterator it = ls.begin(); it != ls.end(); it++) {
        if (m_fsencoding)
            qls.push_back(QString::fromLocal8Bit(it->c_str()));
        else
            qls.push_back(QString::fromUtf8(it->c_str()));
    }
    m_lb->clear();
    m_lb->insertItems(0, qls);
}

void ConfParamSLW::showInputDialog()
{
    bool ok;
    QString s = QInputDialog::getText (this, 
				       "", // title 
				       "", // label, 
				       QLineEdit::Normal, // EchoMode mode
				       "", // const QString & text 
				       &ok);

    if (ok && !s.isEmpty()) {
	QList<QListWidgetItem *>items = 
	    m_lb->findItems(s, Qt::MatchFixedString|Qt::MatchCaseSensitive);
	if (items.empty()) {
	    m_lb->insertItem(0, s);
	    m_lb->sortItems();
	}
    }
}

void ConfParamSLW::deleteSelected()
{
    // We used to repeatedly go through the list and delete the first
    // found selected item (then restart from the beginning). But it
    // seems (probably depends on the qt version), that, when deleting
    // a selected item, qt will keep the selection active at the same
    // index (now containing the next item), so that we'd end up
    // deleting the whole list.
    //
    // Instead, we now build a list of indices, and delete it starting
    // from the top so as not to invalidate lower indices

    vector<int> idxes;
    for (int i = 0; i < m_lb->count(); i++) {
	if (m_lb->item(i)->isSelected()) {
	    idxes.push_back(i);
	}
    }
    for (vector<int>::reverse_iterator it = idxes.rbegin(); 
	 it != idxes.rend(); it++) {
	QListWidgetItem *item = m_lb->takeItem(*it);
	emit entryDeleted(item->text());
	delete item;
    }
}

// "Add entry" dialog for a file name list
void ConfParamDNLW::showInputDialog()
{
    QString s = myGetFileName(true);
    if (!s.isEmpty()) {
	QList<QListWidgetItem *>items = 
	    m_lb->findItems(s, Qt::MatchFixedString|Qt::MatchCaseSensitive);
	if (items.empty()) {
	    m_lb->insertItem(0, s);
	    m_lb->sortItems();
	    QList<QListWidgetItem *>items = 
		m_lb->findItems(s, Qt::MatchFixedString|Qt::MatchCaseSensitive);
	    if (m_lb->selectionMode() == QAbstractItemView::SingleSelection && 
		!items.empty())
		m_lb->setCurrentItem(*items.begin());
	}
    }
}

// "Add entry" dialog for a constrained string list
void ConfParamCSLW::showInputDialog()
{
    bool ok;
    QString s = QInputDialog::getItem (this, // parent
				       "", // title 
				       "", // label, 
				       m_sl, // items, 
				       0, // current = 0
				       false, // editable = true, 
				       &ok);

    if (ok && !s.isEmpty()) {
	QList<QListWidgetItem *>items = 
	    m_lb->findItems(s, Qt::MatchFixedString|Qt::MatchCaseSensitive);
	if (items.empty()) {
	    m_lb->insertItem(0, s);
	    m_lb->sortItems();
	}
    }
}

} // Namespace confgui
