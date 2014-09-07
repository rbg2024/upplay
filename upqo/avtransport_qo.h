
#include <iostream>
using namespace std;

#include <QObject>
#include <QThread>
#include <QDebug>

#include "libupnpp/control/avtransport.hxx"

class AVTMetadata {
public:
    virtual std::string getDidl() const = 0;
};

class AVTransportQO : public QObject, public UPnPClient::VarEventReporter {
Q_OBJECT

public:
    AVTransportQO(UPnPClient::AVTH avt, QObject *parent = 0)
        : QObject(parent), m_srv(avt), m_cursecs(-1)
    {
        m_srv->installReporter(this);
    }

    virtual void changed(const char *nm, int value)
    {
        if (!strcmp(nm, "CurrentTrackDuration")) {
            m_cursecs = value;
        }
            
        qDebug() << "AVT: Changed: " << nm << " (int): " << value;
    }
    virtual void changed(const char *nm, const char *value)
    {
        qDebug() << "AVT: Changed: " << nm << " (char*): " << value;
    }

    virtual void changed(const char *nm, UPnPDirObject meta)
    {
        string s = meta.dump();
        if (!strcmp(nm, "AVTransportURIMetaData")) {
            emit titleChanged(QString::fromUtf8(
                                  meta.m_title.c_str()));
            emit albumChanged(QString::fromUtf8(
                                  meta.m_props["upnp:album"].c_str()));
            emit artistChanged(QString::fromUtf8(
                                   meta.m_props["upnp:artist"].c_str()));
            //m_cursecs = meta.getDurationSeconds();
            qDebug() << "AVT: duration now: " << m_cursecs << " S";
        }
        qDebug() << "AVT: Changed: " << nm << " (dirc): " << s.c_str();
    }

public slots:

    virtual void play() {m_srv->play();}
    virtual void stop() {m_srv->stop();}
    virtual void pause() {m_srv->pause();}

    virtual void changeTrack(const string& uri, const AVTMetadata* md)
    {
        m_srv->setAVTransportURI(uri, md->getDidl());
    }

    // Seek to point. Parameter in percent.
    virtual void seekPC(int pc) 
    {
        qDebug() << "AVT: seekPC " << pc << " %";
        if (m_cursecs > 0) {
            m_srv->seek(UPnPClient::AVTransport::SEEK_ABS_TIME, 
                        (float(pc)/100.0) * m_cursecs); 
        }
    }

signals:
    void titleChanged(QString);
    void albumChanged(QString);
    void artistChanged(QString);

private:
    UPnPClient::AVTH m_srv;
    int m_cursecs;
};

