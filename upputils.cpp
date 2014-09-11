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
#include <string>
using namespace std;

#include <QString>

#include <libupnpp/upnpavutils.hxx>
#include <libupnpp/control/cdircontent.hxx>

#include "HelperStructs/MetaData.h"
#include "upputils.h"

using namespace UPnPClient;
using namespace UPnPP;

// Upnp field to qstring
static QString uf2qs(UPnPDirObject *dop, const char *nm, bool isres)
{
    string val;
    bool ret = isres? dop->getrprop(0, nm, val) : dop->getprop(nm, val);
    if (ret) 
        return QString::fromUtf8(val.c_str());
    else
        return QString();
}

bool udirentToMetadata(UPnPDirObject *dop, MetaData *mdp)
{
    if (dop == 0 || mdp == 0)
        return false;
    // Can't do anything with the ids, they have nothing to do with
    // the upnp id and parentid
    mdp->id = -1;
    mdp->artist_id = -1;
    mdp->album_id = -1;

    mdp->title = QString::fromUtf8(dop->m_title.c_str());
    mdp->artist = uf2qs(dop, "upnp:artist", false);
    mdp->album = uf2qs(dop, "upnp:album", false);
    mdp->rating = 0;
    QString dur = uf2qs(dop, "duration", true); 
    if (!dur.isEmpty()) {
        string sdur((const char*)dur.toUtf8());
        mdp->length_ms = upnpdurationtos(sdur) * 1000;
    } else {
        mdp->length_ms = 0;
    }
    // UPnP DIDL defines a "dc:date" property, YYYY-MM-DD,
    // but I've never seen it set...
    mdp->year = 0;
    if (dop->m_resources.size() == 0) {
        mdp->filepath = "";
    } else {
        mdp->filepath = 
            QString::fromLocal8Bit(dop->m_resources[0].m_uri.c_str());
    }
    mdp->track_num = uf2qs(dop, "upnp:originalTrackNumber", false).toInt();
    mdp->bitrate = uf2qs(dop, "bitrate", true).toInt();
    mdp->is_extern = false;
    mdp->filesize = uf2qs(dop, "size", true).toLongLong();;
    mdp->comment = "";
    mdp->discnumber = 0;
    mdp->n_discs = -1;

    mdp->didl = QString::fromUtf8(dop->getdidl().c_str());

    return true;
}

