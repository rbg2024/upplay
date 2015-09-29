#ifndef UPPLAY_PREFS_H_INCLUDED
#define UPPLAY_PREFS_H_INCLUDED

#include <QWidget>

namespace confgui {
class ConfTabsW;
}

class UPPrefs : public QObject {
    Q_OBJECT

public:

    UPPrefs(QWidget *parent) 
        : m_parent(parent), m_w(0) {
    }

public slots:

    void onShowPrefs();

signals:
    void sig_prefsChanged();

private:
    QWidget *m_parent;
    confgui::ConfTabsW *m_w;
};

#endif
