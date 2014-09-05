
#include <iostream>
using namespace std;

#include <QObject>
#include <QThread>

#include "libupnpp/control/avtransport.hxx"

class AVTransportQO : public QObject, public UPnPClient::VarEventReporter {
Q_OBJECT

public:
    AVTransportQO(UPnPClient::AVTH avt, QObject *parent = 0)
        : QObject(parent), m_srv(avt)
        {
            m_srv->installReporter(this);
        }

    void changed(const char *nm, int value)
        {
//            cout << "Changed: " << nm << " (int): " << value  << endl;
        }
    void changed(const char *nm, const char *value)
        {
//            cout << "Changed: " << nm << " (char*): " << value << endl;
        }

    void changed(const char *nm, UPnPDirContent meta)
        {
            string s("NO CONTENT");
            if (meta.m_items.size() > 0) {
                s = meta.m_items[0].dump();
                if (!strcmp(nm, "AVTransportURIMetaData"))
                    emit titleChanged(QString::fromUtf8(
                                          meta.m_items[0].m_title.c_str()));
                    emit albumChanged(QString::fromUtf8(
                                          meta.m_items[0].m_props["upnp:album"].c_str()));
                    emit artistChanged(QString::fromUtf8(
                                          meta.m_items[0].m_props["upnp:artist"].c_str()));
            }
//            cout << "Changed: " << nm << " (dirc): " << s << endl;
        }

public slots:

signals:
    void titleChanged(QString);
    void albumChanged(QString);
    void artistChanged(QString);

private:
    UPnPClient::AVTH m_srv;
};

