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
        //qDebug() << "AudioScrobbler::maybeScrobble: registered new song";
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
    //qDebug() << "AudioScrobbler::maybeQueue: m_sent" << m_sent <<
    //" artist " << m_curmeta.artist << " " <<
    //" title " << m_curmeta.title << " mS " << m_curmeta.length_ms;
    if(m_sent || m_curmeta.artist.isEmpty() ||
       m_curmeta.title.isEmpty() || m_curmeta.length_ms / 1000 < 5) {
        return;
    }

    if(m_curpos >= 240 ||
       m_curpos >= m_curmeta.length_ms / 2000) {
        //qDebug() << "AudioScrobbler::maybeQueue: queueing" << m_curmeta.title;

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
        //qDebug() << "AudioScrobbler::processQueue: qs " << m_queue.size() <<
        //" fail time " << time(0) - m_lastqfail;
        return;
    }
        
    if (scrobble(m_queue.begin().value())) {
        m_queue.erase(m_queue.begin());
    } else {
        scrobbleFailure();
    }
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

void AudioScrobbler::openURL(const QString& url, const char *data)
{
    if (m_netman == 0 || m_netactive) {
        qDebug() << "AudioScrobbler::openURL: no netman or already active";
        return;
    }
    
    QNetworkRequest request;
    request.setUrl(url);
    m_netactive = true;
    if (data) {
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/x-www-form-urlencoded");
        m_netman->post(request, QByteArray(data));
    } else {
        m_netman->get(request);
    }
}

void AudioScrobbler::replyFinished(QNetworkReply *reply)
{
    QNetworkReply::NetworkError error = reply->error();
    QByteArray qdata;
    if (error == QNetworkReply::NoError) {
        qdata = reply->readAll();
        // qDebug() << "Success: " << qdata;
    } else {
        qDebug() << "Failure" << reply->errorString();
    }
    reply->deleteLater();
    m_netactive = false;
    
    string data((const char *)qdata);
    
    if(data.find("<lfm status=\"ok\">") == std::string::npos) {
        checkFailure(data);
        return;
    }

    if (data.find("<session>") != string::npos &&
        data.find("<key>") != string::npos) {
        // Login Response:
        // <?xml version="1.0" encoding="utf-8"?>
        // <lfm status="ok">
        //   <session>
        //     <name>medoc92</name>
        //     <key>8b654a6cbf3a40a882d90430d398ab92</key>
        //     <subscriber>0</subscriber>
        //   </session>
        // </lfm>
        size_t start, end;
        start = data.find("<key>") + 5;
        end = data.find("</key>");
        m_sessionid = data.substr(start, end-start);
        //qDebug() << "AudioScrobbler: Last.fm handshake ok. SessionID: " <<
        // m_sessionid.c_str();
        m_authed = true;
        m_lastqfail = 0;
    } else if (data.find("<scrobbles") != string::npos &&
        data.find("<scrobble>") != string::npos) {
        // <lfm status="ok">
        //   <scrobbles ignored="0" accepted="1">
        //     <scrobble>
        //       <track corrected="0">De Camino a La Vereda</track>
        //       <artist corrected="0">Buena Vista Social Club</artist>
        //       <album corrected="0">Buena Vista Social Club</album>
        //       <albumArtist corrected="0"></albumArtist>
        //       <timestamp>1459441085</timestamp>
        //       <ignoredMessage code="0"></ignoredMessage>
        //     </scrobble>
        //    </scrobbles>
        // </lfm>
        qDebug() << "Scrobbled successfully.";
    } else {
        qDebug() << "AudioScrobbler: unrecognized response";
    }
}

bool AudioScrobbler::checkFailure(string respdata)
{
    size_t start, end;
    start = respdata.find("<error code=\"") + 13;
    end = respdata.find(">", start) - 1;
    string errorcode = respdata.substr(start, end - start);
    int code = strtol(errorcode.c_str(), 0, 10);

    qDebug() << "AudioScrobbler: error code: " <<  code;

    switch(code) {
    case 3:
        qDebug() << "AudioScrobbler: Invalid Method. This should not happen.";
        return true;
    case 4:
        qDebug() << "AudioScrobbler: Authentication failed. "
            "Please check your login data.";
        return false;
    case 9:
        qDebug() << "AudioScrobbler: Invalid session key. Re-authenticating.";
        m_failcount = 3;
        return true;
    case 10:
        qDebug() << "AudioScrobbler: Invalid API-Key !!";
        return false;
    case 16:
        qDebug() << "AudioScrobbler: service is temporarily unavailable";
        return true;
    case 26:
        qDebug() << "AudioScrobbler: Suspended API key !!";
        return false;
    }
    return false;
}

string AudioScrobbler::createScrobbleMessage(const MetaData& meta)
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
        "&sk=" << m_sessionid;

    array = "";

    // Signature: parameters are ordered alphabetically
    sigmsg << "album" << array << album;
    sigmsg << "api_key" << apiKey;
    sigmsg << "artist" << array << artist;
    sigmsg << "duration" << array << meta.length_ms / 1000;
    sigmsg << "methodtrack.Scrobble";
    sigmsg << "sk" << m_sessionid;
    sigmsg << "timestamp" << array << meta.filesize;
    sigmsg << "track" << array << title;
    sigmsg << apiSecret;

    string digest, sighash;
    MD5String(sigmsg.str(), digest);
    MD5HexPrint(digest, sighash);

    msg << "&api_sig=" << sighash;

    return msg.str();
}

bool AudioScrobbler::scrobble(const MetaData& meta)
{
    if(!m_authed) {
        //qDebug() << "AudioScrobbler::scrobble: handshake not done yet.";
        handshake();
        return false;
    }

    qDebug() << "AudioScrobbler:scrobble: " << meta.title << " " << meta.artist;

    openURL(rootUrl, createScrobbleMessage(meta).c_str());

    return true;
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

    openURL(rootUrl, query.c_str());
}
