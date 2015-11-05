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
#include <cstdlib>

#include <QDebug>
#include <QTime>

#include "randplayer.h"

#include "upadapt/upputils.h"

using namespace std;

static const unsigned int slicesize(30);

RandPlayer::RandPlayer(PlayMode mode,
                       const std::vector<UPnPClient::UPnPDirObject>& entries,
                       QObject *parent)
    : QObject(parent), m_playmode(mode), m_entries(entries)
{
    qsrand(QTime::currentTime().msec());
    if (m_playmode == PM_TRACKS) {
        std::random_shuffle(m_entries.begin(), m_entries.end());
    }
}

static bool sameValues(const string& alb, const string& ctp,
                       UPnPClient::UPnPDirObject& e)
{
    return !e.f2s("upnp:album", false).compare(alb) &&
        !e.f2s("upplay:ctpath", false).compare(ctp);
}

void RandPlayer::playNextSlice()
{
    qDebug() << "RandPlayer: " << m_entries.size() << " remaining";

    if (m_entries.empty()) {
        emit sig_randplay_done();
        return;
    }

    vector<UPnPClient::UPnPDirObject> ents;

    if (m_playmode == PM_TRACKS) {
        // The tracks are already shuffle, just take the front slicesize
        vector<UPnPClient::UPnPDirObject>::iterator last =
            m_entries.size() > slicesize ? m_entries.begin() + slicesize :
            m_entries.end();

        ents.insert(ents.begin(), m_entries.begin(), last);
        m_entries.erase(m_entries.begin(), last);
    } else {
        // Pick a random start, seek back to beginning of group, then
        // forward to end
        double fstart = (double(qrand()) / double(RAND_MAX)) *
            (m_entries.size() - 1);
        int istart = round(fstart);

        // Reference values
        string alb = m_entries[istart].f2s("upnp:album", false);
        string ctpath = m_entries[istart].f2s("upplay:ctpath", false);
        qDebug() << "RandPlayer: albs. istart" << istart << " album " <<
            alb.c_str();

        // Look back to beginning of section
        while (istart > 0) {
            istart--;
            if (!sameValues(alb, ctpath, m_entries[istart])) {
                istart++;
                break;
            }
        }
        qDebug() << "RandPlayer: albs. final istart" << istart;

        // Look forward to end, and store entries
        vector<UPnPClient::UPnPDirObject>::iterator last =
            m_entries.begin() + istart;
        while (last != m_entries.end()) {
            if (!sameValues(alb, ctpath, *last)) {
                break;
            }
            ents.push_back(*last++);
        }
        // Erase used entries.
        m_entries.erase(m_entries.begin() + istart, last);
    }

    qDebug() << "RandPlayer: sending " << ents.size() << " entries to pl";
    MetaDataList mdl;
    mdl.resize(ents.size());
    for (unsigned int i = 0; i <  ents.size(); i++) {
        udirentToMetadata(&ents[i], &mdl[i]);
    }

    emit sig_tracks_to_playlist(mdl);
}
