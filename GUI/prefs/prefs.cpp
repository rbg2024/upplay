
#include <string>

#include <QSettings>
#include <QDebug>

#include "prefs.h"
#include "confgui.h"
#include "sortprefs.h"

using namespace std;
using namespace confgui;

class ConfLinkQS : public confgui::ConfLinkRep {
public:
    ConfLinkQS(const QString& nm)
	: m_nm(nm) {
    }
    virtual ~ConfLinkQS() {
    }

    virtual bool set(const std::string& val) {
        QSettings settings;
        settings.setValue(m_nm, QString::fromUtf8(val.c_str()));
	return true;
    }
    virtual bool get(std::string& val) {
        QSettings settings;
	bool ret = settings.contains(m_nm);
        if (ret) {
            QString qv = settings.value(m_nm).toString();
            val = string((const char*)qv.toUtf8());
        }
	return ret;
    }
private:
    const QString m_nm;
};

class MyConfLinkFactQS : public confgui::ConfLinkFact {
public:
    MyConfLinkFactQS() {
    }
    virtual ConfLink operator()(const QString& nm) {
        ConfLinkRep *lnk = new ConfLinkQS(nm);
        return ConfLink(lnk);
    }
};

static MyConfLinkFactQS lnkfact;

void UPPrefs::onShowPrefs()
{
    qDebug() << "UPPrefs::onShowPrefs(). m_w: " << (void *)m_w;

    if (m_w == 0) {
        m_w = new ConfTabsW(m_parent, "UPPlay Preferences", &lnkfact);
        int idx = m_w->addPanel("Application");
        m_w->addParam(idx, ConfTabsW::CFPT_BOOL, "min2tray", "Close to tray",
                   "Minimize to tray instead of exiting when the main window "
                   "is closed");
        m_w->addParam(idx, ConfTabsW::CFPT_BOOL, "showartwithalb", 
                      "Show some artist information in album lists",
                      "List the beginning of artist info when listing albums.\n"
                      "The exact amount can be adjusted with the "
                      "following entry");
        QSettings settings;
        if (!settings.contains("artwithalblen")) {
            settings.setValue("artwithalblen", 15);
        }
        m_w->addParam(idx, ConfTabsW::CFPT_INT, "artwithalblen", 
                      "Max artist info size in album lists.",
                      "Use 0 for no truncation", 0, 100);
        idx = m_w->addForeignPanel(new SortprefsW(m_w), "Directory Sorting");

        connect(m_w, SIGNAL(sig_prefsChanged()), 
                this, SIGNAL(sig_prefsChanged()));
    }


    m_w->reloadPanels();
    m_w->show();
}
