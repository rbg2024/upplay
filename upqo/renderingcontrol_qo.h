
#include <iostream>
using namespace std;

#include <QObject>
#include <QThread>

#include "libupnpp/control/renderingcontrol.hxx"

class RenderingControlQO : public QObject, public UPnPClient::VarEventReporter {
Q_OBJECT

public:
    RenderingControlQO(UPnPClient::RDCH rdc, QObject *parent = 0)
        : QObject(parent), m_srv(rdc)
        {
            m_srv->installReporter(this);
        }

    void changed(const char *nm, int value)
        {
            cout << "Changed: " << nm << " : " << value << " (int)" << endl;
            if (!strcmp(nm, "Volume")) {
                emit volumeChanged(value);
            } else if (!strcmp(nm, "Mute")) {
                emit muteChanged(value);
            }
        }
    void changed(const char *nm, const char *value)
        {
            cout << "Changed: " << nm << " : " << value << " (char*)" << endl;
        }

    int volume() {return m_srv->getVolume();}
    bool mute()  {return m_srv->getMute();}

public slots:
    void setVolume(int vol)
        {
            cout << "setVolume " << vol << endl;
            m_srv->setVolume(vol);
        }
    void setMute(bool mute)
        {
            cout << "setMute " << mute << endl;
            m_srv->setMute(mute);
        }

signals:
    void volumeChanged(int);
    void muteChanged(bool);

private:
    UPnPClient::RDCH m_srv;
};

