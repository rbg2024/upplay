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
#include <vector>
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
        m_ctobjids.insert(objid);
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
            if (!scanContainer(*m_ctobjids.begin()))
                break;
        }
        if (!m_slices.empty() && !m_slices.back()->m_items.empty()) {
            emit sliceAvailable(&*m_slices.back());
        }

        emit done(m_status);
    }

signals:
    void sliceAvailable(const UPnPClient::UPnPDirContent*);
    void done(int);

private:

    virtual bool scanContainer(const std::string& objid) {
        //qDebug() << "RecursiveReaper::scanCT: objid:" << objid.c_str();

        std::unordered_set<std::string>::iterator thisct = 
            m_ctobjids.find(objid);
        if (thisct == m_ctobjids.end()) {
            qDebug() << "RecursiveReaper::scanCT: dir not found in set";
        }

        int offset = 0;
        int toread = 20; // read small count the first time
        int total = 1000;// Updated on first read.
        int count;

        while (offset < total) {
            UPnPClient::UPnPDirContent& slice = *m_slices.back();
            unsigned int lastctidx = slice.m_containers.size();
            m_status = m_serv->readDirSlice(objid, offset, toread,
                                            slice,  &count, &total);
            if (m_status != UPNP_E_SUCCESS) {
                m_ctobjids.erase(thisct);
                return false;
            }
            offset += count;

            for (auto it = slice.m_containers.begin() + lastctidx;
                 it != slice.m_containers.end(); it++) {
                if (m_allctobjids.find(it->m_id) != m_allctobjids.end()) {
                    qDebug() << "scanContainer: loop detected";
                    continue;
                }
                m_allctobjids.insert(it->m_id);
                m_ctobjids.insert(it->m_id);
            }
            toread = m_serv->goodSliceSize();

            slice.m_containers.clear();
            emit sliceAvailable(&slice);
            m_slices.push_back(new UPnPClient::UPnPDirContent());
        }
        
        m_ctobjids.erase(thisct);
        return true;
    }

    UPnPClient::CDSH m_serv;
    std::unordered_set<std::string> m_ctobjids;
    std::unordered_set<std::string> m_allctobjids;
    int m_status;

public:
    // We use a list (vs vector) so that existing element addresses
    // are unchanged when we append
    std::list<UPnPClient::UPnPDirContent*> m_slices;
};

#endif /* _RREAPER_H_INCLUDED_ */
