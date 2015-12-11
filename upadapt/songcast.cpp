/* Copyright (C) 2015 J.F.Dockes
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
#include "songcast.h"

#include <iostream>
#include <vector>

#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QTimer>
#include <QDebug>

#include "libupnpp/control/linnsongcast.hxx"

#include "GUI/songcast/songcastdlg.h"
#include "upadapt/upputils.h"

using namespace std;
using namespace UPnPClient;
using namespace Songcast;

static const int RCVWPERLINE = 3;
static const int SNDWPERLINE = 2;

class SongcastTool::Internal {
public:
    SongcastDLG *dlg;
    vector<ReceiverState> receivers;
    vector<SenderState> senders;
};

SongcastTool::SongcastTool(SongcastDLG *dlg, QObject *parent)
    : QObject(parent)
{
    m = new Internal;
    m->dlg = dlg;
    connect(m->dlg, SIGNAL(sig_apply(SongcastDLG*)),
            this, SLOT(onSongcastApply()));
    connect(m->dlg, SIGNAL(accepted()),
            this, SLOT(onSongcastApply()));
    initControls();
}

void SongcastTool::initControls()
{
    QLayoutItem *child;
    while ((child = m->dlg->sndGridLayout->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }
    while ((child = m->dlg->rcvGridLayout->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }
    m->dlg->sndGridLayout->update();
    m->dlg->rcvGridLayout->update();
    listSenders(m->senders);
    listReceivers(m->receivers);

    if (m->senders.size() == 0) {
        QLabel *lbl = new QLabel(tr("No Songcast Senders found"), m->dlg);
        m->dlg->sndGridLayout->addWidget(lbl, 0, 0, 1, 1);
    } else {
        QLabel *lbl;
        QRadioButton *btn;
            
        for (unsigned int i = 0; i < m->senders.size(); i++) {
            btn = new QRadioButton(m->dlg->sndGroupBox);
            connect(btn, SIGNAL(toggled(bool)), this, SLOT(enableOnButtons()));
            lbl = new QLabel(m->dlg->sndGroupBox);
            lbl->setText(u8s2qs(m->senders[i].nm));
            m->dlg->sndGridLayout->addWidget(btn, i, 0, 1, 1);
            m->dlg->sndGridLayout->addWidget(lbl, i, 1, 1, 1);
        }
    }

    if (m->receivers.size() == 0) {
        QLabel *lbl = new QLabel(tr("No Songcast Receivers found"), m->dlg);
        m->dlg->rcvGridLayout->addWidget(lbl, 0, 0, 1, 1);
    } else {
        QLabel *lbl;
        UncheckCheckBox *on, *off;
            
        for (unsigned int i = 0; i < m->receivers.size(); i++) {
            on = new UncheckCheckBox(tr("Link to selected Sender"),
                               m->dlg->rcvGroupBox);
            off = new UncheckCheckBox(tr("Unlink"), m->dlg->rcvGroupBox);
            lbl = new QLabel(m->dlg->rcvGroupBox);
            QString rcvdesc(u8s2qs(m->receivers[i].nm));
            ReceiverState::SCState st(m->receivers[i].state);
            bool isconnected = st == ReceiverState::SCRS_STOPPED ||
                st ==  ReceiverState::SCRS_PLAYING;
            if (isconnected) {
                string snm = senderNameFromUri(m->receivers[i].uri);
                rcvdesc += tr(" (connected to: ") + u8s2qs(snm) + ")";
            }
            lbl->setText(rcvdesc);
            m->dlg->rcvGridLayout->addWidget(off, i, 0, 1, 1);
            on->setEnabled(false);
            off->setEnabled(isconnected);
            m->dlg->rcvGridLayout->addWidget(on,  i, 1, 1, 1);
            m->dlg->rcvGridLayout->addWidget(lbl, i, 2, 1, 1);
            connect(on, SIGNAL(toggled(bool)),
                    off, SLOT(setUnChecked(bool)));
            connect(off, SIGNAL(toggled(bool)),
                    on, SLOT(setUnChecked(bool)));
        }
    }
}

SongcastDLG *SongcastTool::dlg()
{
    return m->dlg;
}

string SongcastTool::senderNameFromUri(const string& uri)
{
    for (unsigned int i = 0; i < m->senders.size(); i++) {
        if (uri == m->senders[i].uri)
            return m->senders[i].nm;
    }
    return string();
}

int SongcastTool::selectedSenderIdx()
{
    int senderidx = -1;
    // Note: quite unbelievably, gridLayout::rowCount() is not the number
    // of actual rows, but the size of the internal allocation. So can't use it
    // after deleting rows...
    for (int i = 0; i < m->dlg->sndGridLayout->count() / SNDWPERLINE; i++) {
        QRadioButton *btn = (QRadioButton*)
            m->dlg->sndGridLayout->itemAtPosition(i, 0)->widget();
        if (btn->isChecked()) {
            senderidx = i;
            break;
        }
    }
    return senderidx;
}

void SongcastTool::onSongcastApply()
{
    int senderidx = selectedSenderIdx();
    // Note: quite unbelievably, gridLayout::rowCount() is not the number
    // of actual rows, but the size of the internal allocation. So can't use it
    // after deleting rows...
    for (int i = 0; i < m->dlg->rcvGridLayout->count() / RCVWPERLINE; i++) {
        UncheckCheckBox *off = (UncheckCheckBox*)
            m->dlg->rcvGridLayout->itemAtPosition(i, 0)->widget();
        UncheckCheckBox *on = (UncheckCheckBox*)
            m->dlg->rcvGridLayout->itemAtPosition(i, 1)->widget();

        if (off->isChecked()) {
            stopReceiver(m->receivers[i]);
        } else  if (senderidx != -1  && on->isChecked()) {
            setReceiverPlaying(m->receivers[i],
                               m->senders[senderidx].uri,
                               m->senders[senderidx].meta);
        }
    }
    QTimer::singleShot(500, this, SLOT(syncReceivers()));
}

// This is called when a sender is selected.
void SongcastTool::enableOnButtons()
{
    int senderidx = selectedSenderIdx();
    // Note: quite unbelievably, gridLayout::rowCount() is not the
    // number of actual rows, but the size of the internal
    // allocation. So can't use it after deleting rows...
    qDebug() << "SongcastTool::enableOnButtons() rows: " <<
        m->dlg->rcvGridLayout->count() / RCVWPERLINE;
    for (int i = 0; i < m->dlg->rcvGridLayout->count() / RCVWPERLINE; i++) {
        ReceiverState::SCState st(m->receivers[i].state);
        bool isconnected = st == ReceiverState::SCRS_STOPPED ||
            st ==  ReceiverState::SCRS_PLAYING;
        UncheckCheckBox *on = (UncheckCheckBox *)
            m->dlg->rcvGridLayout->itemAtPosition(i, 1)->widget();
        on->setEnabled(!isconnected && senderidx >= 0);
    }
}

// Synchronize ui state with actual receiver state. This will not list
// new devices or delete old ones.
void SongcastTool::syncReceivers()
{
    cerr << "SongcastTool::syncReceivers()\n";
    int senderidx = selectedSenderIdx();
    QLabel *lbl;
    UncheckCheckBox *on, *off;
            
    for (unsigned int i = 0; i < m->receivers.size(); i++) {
        off = (UncheckCheckBox *)
            m->dlg->rcvGridLayout->itemAtPosition(i, 0)->widget();
        on = (UncheckCheckBox *)
            m->dlg->rcvGridLayout->itemAtPosition(i, 1)->widget();
        lbl = (QLabel*)
            m->dlg->rcvGridLayout->itemAtPosition(i, 2)->widget();
        on->setChecked(false);
        off->setChecked(false);
        
        string udn = m->receivers[i].UDN;
        getReceiverState(udn, m->receivers[i], false);

        QString rcvdesc(u8s2qs(m->receivers[i].nm));
        ReceiverState::SCState st(m->receivers[i].state);
        bool isconnected = st == ReceiverState::SCRS_STOPPED ||
            st ==  ReceiverState::SCRS_PLAYING;
        if (isconnected) {
            string snm = senderNameFromUri(m->receivers[i].uri);
            rcvdesc += tr(" (connected to: ") + u8s2qs(snm) + ")";
            on->setEnabled(false);
        } else {
            on->setEnabled(senderidx >= 0);
        }
        lbl->setText(rcvdesc);
        off->setEnabled(isconnected);
    }
}
