/* Copyright (C) 2014 J.F.Dockes
 *	 This program is free software; you can redistribute it and/or modify
 *	 it under the terms of the GNU General Public License as published by
 *	 the Free Software Foundation; either version 2 of the License, or
 *	 (at your option) any later version.
 *
 *	 This program is distributed in the hope that it will be useful,
 *	 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *
 *	 You should have received a copy of the GNU General Public License
 *	 along with this program; if not, write to the
 *	 Free Software Foundation, Inc.,
 *	 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _RREAPER_H_INCLUDED_
#define _RREAPER_H_INCLUDED_

#include <string>
#include <list>
#include <queue>
#include <iostream>
#include <unordered_set>

#include <QDebug>
#include <QThread>

#include <upnp/upnp.h>

#include "libupnpp/control/cdirectory.hxx"

class RecursiveReaper : public QThread {
    Q_OBJECT;

 public: 
    RecursiveReaper(UPnPClient::CDSH server, std::string objid, 
                       QObject *parent = 0)
        : QThread(parent), m_serv(server)
    {
        m_ctobjids.push(objid);
        m_allctobjids.insert(objid);
    }

    ~RecursiveReaper()
    {
    }

    virtual void run() {
        qDebug() << "RecursiveReaper::run";
        m_slices.push_back(new UPnPClient::UPnPDirContent());
	m_status = UPNP_E_SUCCESS;
        while (!m_ctobjids.empty()) {
            // We don't stop on a container scan error, minimserver for one 
            // sometimes has dialog hiccups with libupnp, this is not fatal.
            scanContainer(m_ctobjids.front());
            m_ctobjids.pop();
        }
        if (!m_slices.empty() && !m_slices.back()->m_items.empty()) {
            emit sliceAvailable(&*m_slices.back());
        }

        emit done(m_status);
        qDebug() << "RecursiveReaper::done";
    }

signals:
    void sliceAvailable(const UPnPClient::UPnPDirContent*);
    void done(int);

private:

    virtual bool scanContainer(const std::string& objid) {
        //qDebug() << "RecursiveReaper::scanCT: objid:" << objid.c_str();

        int offset = 0;
        int toread = 20; // read small count the first time
        int total = 1000;// Updated on first read.
        int count;

        while (offset < total) {
            UPnPClient::UPnPDirContent& slice = *m_slices.back();

            unsigned int lastctidx = slice.m_containers.size();

            // Read entries
            m_status = m_serv->readDirSlice(objid, offset, toread,
                                            slice,  &count, &total);
            if (m_status != UPNP_E_SUCCESS) {
                return false;
            }
            offset += count;

            // Put containers aside for later exploration
            for (auto it = slice.m_containers.begin() + lastctidx;
                 it != slice.m_containers.end(); it++) {
                if (it->m_title.empty())
                    continue;
                if (m_serv->getKind() == 
                    UPnPClient::ContentDirectory::CDSKIND_MINIM &&
                    it->m_title.size() >= 2 && it->m_title[0] == '>' && 
                    it->m_title[1] == '>') {
                    continue;
                }
                if (m_allctobjids.find(it->m_id) != m_allctobjids.end()) {
                    qDebug() << "scanContainer: loop detected";
                    continue;
                }
                //qDebug()<< "scanContainer: pushing objid " << it->m_id.c_str()
                // << " title " << it->m_title.c_str();
                m_allctobjids.insert(it->m_id);
                m_ctobjids.push(it->m_id);
            }
            slice.m_containers.clear();

            // Make items available
            if (!slice.m_items.empty()) {
                qDebug() << "RecursiveReaper::scanCT got " << 
                    slice.m_items.size() << " items";
                emit sliceAvailable(&slice);
                m_slices.push_back(new UPnPClient::UPnPDirContent());
            }
            toread = m_serv->goodSliceSize();
        }
        
        return true;
    }

    UPnPClient::CDSH m_serv;
    std::queue<std::string> m_ctobjids;
    std::unordered_set<std::string> m_allctobjids;
    int m_status;

public:
    // We use a list (vs vector) so that existing element addresses
    // are unchanged when we append
    std::list<UPnPClient::UPnPDirContent*> m_slices;
};

#endif /* _RREAPER_H_INCLUDED_ */
