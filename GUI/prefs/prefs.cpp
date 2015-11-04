
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

        // Close to tray ?
        m_w->addParam(idx, ConfTabsW::CFPT_BOOL, "min2tray", "Close to tray",
                   "Minimize to tray instead of exiting when the main window "
                   "is closed");

        // Truncate artist information in directory listings?
        ConfParamW *b1 =
            m_w->addParam(idx, ConfTabsW::CFPT_BOOL, "truncateartistindir", 
                          "Truncate artist information in track lists",
                          "Truncate very long artist info so that the table "
                          "does not become weird");
        QSettings settings;
        QString pname("truncateartistlen");
        if (!settings.contains(pname)) {
            settings.setValue(pname, 30);
        }
        ConfParamW *w1 =
            m_w->addParam(idx, ConfTabsW::CFPT_INT, pname, 
                          "Max artist info size in track lists.",
                          "", 0, 100);
        m_w->enableLink(b1, w1);


        // Show some artist information with albums ?
        b1 = m_w->addParam(idx, ConfTabsW::CFPT_BOOL, "showartwithalb", 
                           "Show some artist information in album lists",
                           "List the beginning of artist info when listing "
                           "albums.\nThe exact amount can be adjusted with the "
                           "following entry");
        pname = "artwithalblen";
        if (!settings.contains(pname)) {
            settings.setValue(pname, 15);
        }
        w1 = m_w->addParam(idx, ConfTabsW::CFPT_INT, pname, 
                           "Max artist info size in album lists.",
                           "", 0, 100);
        m_w->enableLink(b1, w1);
        

        idx = m_w->addForeignPanel(new SortprefsW(m_w), "Directory Sorting");

        connect(m_w, SIGNAL(sig_prefsChanged()), 
                this, SIGNAL(sig_prefsChanged()));
    }


    m_w->reloadPanels();
    m_w->show();
}
