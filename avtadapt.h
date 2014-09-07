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

#ifndef _AVTADAPT_H_INCLUDED_
#define _AVTADAPT_H_INCLUDED_

#include <iostream>
using namespace std;

#include <QDebug>
#include "HelperStructs/MetaData.h"

#include "upqo/avtransport_qo.h"
#include "upputils.h"

class MetaDataAdaptor : public AVTMetadata {
public:
    MetaDataAdaptor(const MetaData &md)
    : m_md(md)
        {}
    virtual ~MetaDataAdaptor() {}

    virtual std::string getDidl() const
        {
            return qs2utf8s(m_md.didl);
        }

private:
    const MetaData& m_md;
};

class AVTPlayer : public AVTransportQO {
Q_OBJECT
public:
    AVTPlayer(UPnPClient::AVTH avt, QObject *parent = 0)
        : AVTransportQO(avt, parent)
        {
        }

public slots:
    void changeTrack(const MetaData& md, int pos_secs, bool playnow)
    {
        cerr << "MetaDataAdaptor:: changetrack" << endl;
        MetaDataAdaptor mad(md);
        AVTransportQO::changeTrack(qs2utf8s(md.filepath), &mad);
        AVTransportQO::play();
    }

};


#endif /* _AVTADAPT_H_INCLUDED_ */
