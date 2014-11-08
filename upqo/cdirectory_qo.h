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

#ifndef _DIRREADER_H_INCLUDED_
#define _DIRREADER_H_INCLUDED_

#include <string>
#include <vector>
#include <iostream>

#include <QThread>
#include <QDebug>

#include <upnp/upnp.h>

#include "libupnpp/control/cdirectory.hxx"

class ContentDirectoryQO : public QThread {
    Q_OBJECT;

 public: 
    ContentDirectoryQO(UPnPClient::CDSH server, std::string objid, 
                       std::string ss = std::string(), QObject *parent = 0)
        : QThread(parent), m_serv(server), m_objid(objid), m_searchstr(ss) {
    }

    ~ContentDirectoryQO() {
        for (auto entry = m_slices.begin(); entry != m_slices.end(); entry++)
            delete *entry;
    }

    virtual void run() {
        qDebug() << "ContentDirectoryQO::run. Search string: " << 
            m_searchstr.c_str();
	int offset = 0;
        int toread = 20; // read small count the first time
	int total = 1000;// Updated on first read.
        int count;

	while (offset < total) {
            UPnPClient::UPnPDirContent *slice = 
                new UPnPClient::UPnPDirContent();
            if (slice == 0) {
                m_status = UPNP_E_OUTOF_MEMORY;
                emit done(m_status);
                return;
            }
            qDebug() << "dirreader: reading " << toread << " total " << total;
            if (m_searchstr.empty()) {
                m_status = m_serv->readDirSlice(m_objid, offset, toread,
                                                *slice,  &count, &total);
            } else {
                m_status = m_serv->searchSlice(m_objid, m_searchstr,
                                               offset, toread,
                                               *slice,  &count, &total);
            }
            if (m_status != UPNP_E_SUCCESS) {
                emit done(m_status);
                return;
            }
            offset += count;
            m_slices.push_back(slice);
            emit sliceAvailable(slice);

            toread = m_serv->goodSliceSize();
	}
        emit done(m_status);
	m_status = UPNP_E_SUCCESS;
    }

    const std::string& getObjid() {return m_objid;}
    UPnPClient::ContentDirectory::ServiceKind getKind() {
        return m_serv->getKind();
    }

signals:
    void sliceAvailable(const  UPnPClient::UPnPDirContent *);
    void done(int);

private:
    UPnPClient::CDSH m_serv;
    std::string m_objid;
    std::string m_searchstr;
    // We use a list (vs vector) so that existing element addresses
    // are unchanged when we append
    std::list< UPnPClient::UPnPDirContent*> m_slices;
    int m_status;
};

#endif /* _DIRREADER_H_INCLUDED_ */
