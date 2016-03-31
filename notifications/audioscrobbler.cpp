/* Copyright (C) 2016 J.F.Dockes
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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>

#include <sstream>

#include "audioscrobbler.h"
#include "upadapt/upputils.h"
#include "upadapt/md5.hxx"
#include "HelperStructs/Helper.h"

using namespace UPnPP;

static const QString rootUrl("http://ws.audioscrobbler.com/2.0/");
// Values for upplay
static const string apiKey("48b8a81fe93a4532efb4b713c8ad8e00");
static const string apiSecret("d5bb8eb0dafce36e20ab6d14deccb264");

AudioScrobbler::AudioScrobbler(QObject *parent)
        : QObject(parent), m_curpos(0), m_starttime(0), m_sent(false),
          m_lastqfail(0), m_failcount(0), m_authed(false), m_netactive(false)
{

    m_netman = new QNetworkAccessManager(this);
    connect(m_netman, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

void AudioScrobbler::maybeScrobble(time_t reltime, const MetaData& meta)
{
    // new song? Pb: the Uri changes slightly before the metadata, so
    // that if we use an uri change to update the meta, we get the one
    // from the previous song. So have to use the meta itself no big deal
    // In addition, duration also changes before the title ! Best to wait
    // a few secs play time before we do anything
    if(reltime >= 3 && (!meta.title.isEmpty() && !meta.artist.isEmpty()) && 
       (m_curmeta.title.compare(meta.title) ||
        m_curmeta.album.compare(meta.album) ||
        m_curmeta.artist.compare(meta.artist))) {

        m_curmeta = meta;
        m_curpos = reltime;
        m_starttime = time(0);
        m_sent = false;
        qDebug() << "AudioScrobbler::maybeScrobble: registered new song";
    }

    if(m_curpos != reltime) {
        // song playing
        m_curpos = reltime;
        maybeQueue();
    }

    processQueue();
}

void AudioScrobbler::maybeQueue()
{
    qDebug() << "AudioScrobbler::maybeQueue: m_sent" << m_sent <<
        " artist " << m_curmeta.artist << " " <<
        " title " << m_curmeta.title << " mS " << m_curmeta.length_ms;
    if(m_sent || m_curmeta.artist.isEmpty() ||
       m_curmeta.title.isEmpty() || m_curmeta.length_ms / 1000 < 5) {
        return;
    }

    if(m_curpos >= 240 ||
       m_curpos >= m_curmeta.length_ms / 2000) {
        qDebug() << "AudioScrobbler::maybeQueue: queueing" << m_curmeta.title;

        MetaData meta = m_curmeta;
        // Store starttime in meta filesize which is not sent to last.fm
        meta.filesize = m_starttime;
        m_queue[meta.title + meta.artist + meta.album] = meta;
        m_sent = true;
    }
}

// This gets called once per second. We don't try to flush the queue
// faster than this, so do at most one send per call
void AudioScrobbler::processQueue()
{
    if (m_queue.isEmpty() || (time(0) - m_lastqfail < 300)) {
        return;
    }
        
    if (scrobble(m_queue.begin().value())) {
        m_queue.erase(m_queue.begin());
    } else {
        scrobbleFailure();
    }
}


void AudioScrobbler::openURL(const QString& url, const QByteArray *data)
{
    if (m_netman == 0 || m_netactive)
        return;
    
    QNetworkRequest request;
    request.setUrl(url);
    m_netactive = true;
    if (data) {
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/x-www-form-urlencoded");
        m_netman->post(request, *data);
    } else {
        m_netman->get(request);
    }
}

void AudioScrobbler::replyFinished(QNetworkReply *reply)
{

    QNetworkReply::NetworkError error = reply->error();
    if (error == QNetworkReply::NoError) {
        qDebug() << "Success" << reply->readAll();
    } else {
        qDebug() << "Failure" << reply->errorString();
    }
    reply->deleteLater();

#if 0
    if(_response.find("<lfm status=\"ok\">") != std::string::npos) {
        size_t start, end;
        start = _response.find("<key>") + 5;
        end = _response.find("</key>");
        _sessionid = _response.substr(start, end-start);
        iprintf("%s%s", "Last.fm handshake successful. SessionID: ", _sessionid.c_str());
        _authed = true;
    } else if(_response.find("<lfm status=\"failed\">") != std::string::npos) {
        CheckFailure(_response);
        exit(EXIT_FAILURE);
    }
#endif
}

QString AudioScrobbler::createScrobbleMessage(const MetaData& meta)
{
    stringstream msg, sigmsg ;
    string artist, title, album, array = "=";

    artist = qs2utf8s(meta.artist);
    title = qs2utf8s(meta.title);
    album = qs2utf8s(meta.album);

    msg << "&album" << array << escapeHtml(album) <<
        "&api_key=" << apiKey << 
        "&artist" << array << escapeHtml(artist) << 
        "&duration" << array << meta.length_ms / 1000 << 
        "&method=track.Scrobble" << 
        "&timestamp" << array << meta.filesize << 
        "&track" << array << escapeHtml(title) << 
        "&sk=" << qs2utf8s(m_sessionid);

    array = "";

    // Signature: parameters are ordered alphabetically
    sigmsg << "album" << array << album;
    sigmsg << "api_key" << apiKey;
    sigmsg << "artist" << array << artist;
    sigmsg << "duration" << array << meta.length_ms / 1000;
    sigmsg << "methodtrack.Scrobble";
    sigmsg << "sk" << qs2utf8s(m_sessionid);
    sigmsg << "timestamp" << array << meta.filesize;
    sigmsg << "track" << array << title;
    sigmsg << apiSecret;

    string digest, sighash;
    MD5String(sigmsg.str(), digest);
    MD5HexPrint(digest, sighash);

    msg << "&api_sig=" << sighash;

    return u8s2qs(msg.str());
}

void AudioScrobbler::scrobbleFailure()
{
    m_lastqfail = time(0);
    m_failcount += 1;
    if(m_failcount >= 3) {
        m_failcount = 0;
        handshake();
    }
}

#if 0
bool CAudioScrobbler::CheckFailure(std::string response)
{
    bool retval = false;

    size_t start, end;
    start = _response.find("<error code=\"")+13;
    end = _response.find(">", start)-1;
    std::string errorcode = _response.substr(start, end-start);
    int code = strtol(errorcode.c_str(), 0, 10);

    eprintf("%s%i", "Code: ", code);

    switch(code) {
    case 3:
        eprintf("Invalid Method. This should not happen.");
        retval = true;
        break;
    case 4:
        eprintf("Authentication failed. Please check your login data.");
        exit(EXIT_FAILURE);
    case 9:
        eprintf("Invalid session key. Re-authenticating.");
        retval = true;
        _failcount = 3;
        break;
    case 10:
        eprintf("Invalid API-Key. Let's bugger off.");
        exit(EXIT_FAILURE);
    case 16:
        eprintf("The service is temporarily unavailable, we will try again later..");
        retval = true;
        break;
    case 26:
        eprintf("Uh oh. Suspended API key - Access for your account has been suspended, please contact Last.fm");
        exit(EXIT_FAILURE);
    }

    return retval;
}
#endif

bool AudioScrobbler::scrobble(const MetaData&)
{
    bool retval = false;
    if(!m_authed) {
        qDebug() << "AudioScrobbler::scrobble: handshake hasn't been done yet.";
        handshake();
        return retval;
    }

#if 0
    iprintf("Scrobbling: %s - %s", entry.getSong().getArtist().c_str(), entry.getSong().getTitle().c_str());

    OpenURL(ROOTURL, CreateScrobbleMessage(0, entry).c_str());
    if(_response.find("<lfm status=\"ok\">") != std::string::npos) {
        iprintf("%s", "Scrobbled successfully.");
        retval = true;
    } else if (_response.find("<lfm status=\"failed\">") != std::string::npos) {
        eprintf("%s%s", "Last.fm returned an error while scrobbling:\n", _response.c_str());
        if(CheckFailure(_response))
            Failure();
    }
#endif

    return retval;
}

void AudioScrobbler::handshake()
{
    QSettings settings;
    QString username = settings.value("lastfmusername").toString().toLower();
    QString password = settings.value("lastfmpassword").toString();

    string digest, authtoken;
    MD5String(qs2utf8s(username+password), digest);
    MD5HexPrint(digest, authtoken);

    string query, sig;
    query = string("method=auth.getMobileSession&username=") +
        qs2utf8s(username) +
        "&authToken=" + authtoken +
        "&api_key=" + apiKey;

    sig = string("api_key") + apiKey +
        "authToken" + authtoken +
        "methodauth.getMobileSessionusername" + qs2utf8s(username) +
        apiSecret;

    string sighash;
    MD5String(sig, digest);
    MD5HexPrint(digest, sighash);

    query += "&api_sig=" + sighash;

    m_qdata = query.c_str();
    openURL(rootUrl, &m_qdata);
}
