/* Copyright (C) 2007 J.F.Dockes
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
#ifndef _confgui_h_included_
#define _confgui_h_included_
/**
 * This file defines a number of simple classes (virtual base: ConfParamW) 
 * which let the user input configuration parameters. 
 *
 * Subclasses are defined for entering different kind of data, ie a string, 
 * a file name, an integer, etc.
 *
 * Each configuration gui object is linked to the configuration data through
 * a "link" object which knows the details of interacting with the actual
 * configuration data, like the parameter name, the actual config object, 
 * the method to call etc.
 * 
 * The link object is set when the input widget is created and cannot be 
 * changed.
 *
 * The widgets are typically linked to a temporary configuration object, which
 * is then copied to the actual configuration if the data is accepted, or
 * destroyed and recreated as a copy if Cancel is pressed (you have to 
 * delete/recreate the widgets in this case as the links are no longer valid).
 */

#include <string>
#include <limits.h>

#include <memory>
#include <vector>

#include <QString>
#include <QWidget>
#include <QDialog>

class ConfNull;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QHBoxLayout;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QVBoxLayout;

namespace confgui {

/** Interface between the GUI widget and the config storage mechanism: */
class ConfLinkRep {
public:
    virtual ~ConfLinkRep() {}
    virtual bool set(const std::string& val) = 0;
    virtual bool get(std::string& val) = 0;
};
typedef std::shared_ptr<ConfLinkRep> ConfLink;

/** Link maker class */
class ConfLinkFact {
public:
    virtual ~ConfLinkFact() {}
    virtual ConfLink operator()(const QString& nm) = 0;
};

class ConfPanelWIF {
public:
    virtual ~ConfPanelWIF() {}
    virtual void storeValues() = 0;
    virtual void loadValues() = 0;
};

/** The top level widget has tabs, each tab/panel has multiple widgets
 * for setting parameter values 
 */
class ConfPanelW;
class ConfTabsW : public QDialog {
    Q_OBJECT;

public:
    ConfTabsW(QWidget *parent, const QString& title, ConfLinkFact *linkfact);

    enum ParamType {CFPT_BOOL, CFPT_INT, CFPT_STR, CFPT_CSTR, CFPT_FN,
                    CFPT_STRL, CFPT_DNL, CFPT_CSTRL};

    /** Add tab and return its identifier / index */
    int addPanel(const QString& title);

    /** Add foreign tab where we only know to call loadvalues/storevalues.
     * The object has to derive from QWidget */
    int addForeignPanel(ConfPanelWIF* w, const QString& title);

    /** Add parameter setter to specified tab */
    void addParam(int tabindex, ParamType tp, 
                  const QString& varname, const QString& label,
                  const QString& tooltip, int isdirorminval = 0, 
                  int maxval = 0, const QStringList* sl = 0);
                                                
public slots:
    void acceptChanges();
    void rejectChanges();
    void reloadPanels();

signals: 
    void sig_prefsChanged();

private:
    ConfLinkFact *m_makelink;
    std::vector<ConfPanelW *> m_panels;
    std::vector<ConfPanelWIF *> m_widgets;
    QTabWidget       *tabWidget;
    QDialogButtonBox *buttonBox;
};

/////////////////////////////////////////////////
// All the rest could be moved to the C++ file ?

/** A panel/tab contains multiple controls for parameters */
class ConfPanelW : public QWidget {
    Q_OBJECT
public:
    ConfPanelW(QWidget *parent);
    void addWidget (QWidget *w);
    void storeValues();
    void loadValues();
private:
    QVBoxLayout *m_vboxlayout;
    std::vector<QWidget *> m_widgets;
};

/** Config panel element: manages one configuration
 *  parameter. Subclassed for specific parameter types.
 */
class ConfParamW : public QWidget {
    Q_OBJECT

public:
    ConfParamW(QWidget *parent, ConfLink cflink)
        : QWidget(parent), m_cflink(cflink), m_fsencoding(false) {
    }
    virtual void loadValue() = 0;
    virtual void setFsEncoding(bool onoff) {
        m_fsencoding = onoff;
    }

public slots:
    virtual void setEnabled(bool) = 0;
    virtual void storeValue() = 0;

protected:
    ConfLink     m_cflink;
    QHBoxLayout *m_hl;
    // File names are encoded as local8bit in the config files. Other
    // are encoded as utf-8
    bool         m_fsencoding;
    virtual bool createCommon(const QString& lbltxt, const QString& tltptxt);
    void setValue(const QString& newvalue);
    void setValue(int newvalue);
    void setValue(bool newvalue);
};

//////// Widgets for setting the different types of configuration parameters:

/**  Boolean */
class ConfParamBoolW : public ConfParamW {
    Q_OBJECT
    public:
    ConfParamBoolW(QWidget *parent, ConfLink cflink, 
                   const QString& lbltxt,
                   const QString& tltptxt);
    virtual void loadValue();
    virtual void storeValue();
public slots:
    virtual void setEnabled(bool i) {
        if(m_cb) 
            ((QWidget*)m_cb)->setEnabled(i);
    }
public:
    QCheckBox *m_cb;
};

// Int
class ConfParamIntW : public ConfParamW {
    Q_OBJECT
    public:
    // The default value is only used if none exists in the sample
    // configuration file. Defaults are normally set in there.
    ConfParamIntW(QWidget *parent, ConfLink cflink, 
                  const QString& lbltxt,
                  const QString& tltptxt,
                  int minvalue = INT_MIN, 
                  int maxvalue = INT_MAX,
                  int defaultvalue = 0);
    virtual void loadValue();
    virtual void storeValue();
public slots:
    virtual void setEnabled(bool i) {
        if(m_sb) 
            ((QWidget*)m_sb)->setEnabled(i);
    }
protected:
    QSpinBox *m_sb;
    int       m_defaultvalue;
};

// Arbitrary string
class ConfParamStrW : public ConfParamW {
    Q_OBJECT
    public:
    ConfParamStrW(QWidget *parent, ConfLink cflink, 
                  const QString& lbltxt,
                  const QString& tltptxt);
    virtual void loadValue();
    virtual void storeValue();
public slots:
    virtual void setEnabled(bool i) {
        if (m_le) ((QWidget*)m_le)->setEnabled(i);
    }
protected:
    QLineEdit *m_le;
};

// Constrained string: choose from list
class ConfParamCStrW : public ConfParamW {
    Q_OBJECT
    public:
    ConfParamCStrW(QWidget *parent, ConfLink cflink, 
                   const QString& lbltxt,
                   const QString& tltptxt, const QStringList& sl);
    virtual void loadValue();
    virtual void storeValue();
public slots:
    virtual void setEnabled(bool i) {
        if (m_cmb) ((QWidget*)m_cmb)->setEnabled(i);
    }
protected:
    QComboBox *m_cmb;
};

// File name
class ConfParamFNW : public ConfParamW {
    Q_OBJECT
    public:
    ConfParamFNW(QWidget *parent, ConfLink cflink, 
                 const QString& lbltxt,
                 const QString& tltptxt, bool isdir = false);
    virtual void loadValue();
    virtual void storeValue();
protected slots:
    void showBrowserDialog();
public slots:
    virtual void setEnabled(bool i) {
        if(m_le) ((QWidget*)m_le)->setEnabled(i);
        if(m_pb) ((QWidget*)m_pb)->setEnabled(i);
    }
protected:
    QLineEdit *m_le;
    QPushButton *m_pb;
    bool       m_isdir;
};

// String list
class ConfParamSLW : public ConfParamW {
    Q_OBJECT
    public:
    ConfParamSLW(QWidget *parent, ConfLink cflink, 
                 const QString& lbltxt,
                 const QString& tltptxt);
    virtual void loadValue();
    virtual void storeValue();
    QListWidget *getListBox() {return m_lb;}
	
public slots:
    virtual void setEnabled(bool i) {
        if (m_lb) 
            ((QWidget*)m_lb)->setEnabled(i);
    }
protected slots:
    virtual void showInputDialog();
    void deleteSelected();
signals:
    void entryDeleted(QString);
protected:
    QListWidget *m_lb;
    void listToConf();
};

// Dir name list
class ConfParamDNLW : public ConfParamSLW {
    Q_OBJECT
    public:
    ConfParamDNLW(QWidget *parent, ConfLink cflink, 
                  const QString& lbltxt,
                  const QString& tltptxt)
        : ConfParamSLW(parent, cflink, lbltxt, tltptxt) {
        m_fsencoding = true;
    }
protected slots:
    virtual void showInputDialog();
};

// Constrained string list (chose from predefined)
class ConfParamCSLW : public ConfParamSLW {
    Q_OBJECT
    public:
    ConfParamCSLW(QWidget *parent, ConfLink cflink, 
                  const QString& lbltxt,
                  const QString& tltptxt,
                  const QStringList& sl)
        : ConfParamSLW(parent, cflink, lbltxt, tltptxt), m_sl(sl) {
    }
protected slots:
    virtual void showInputDialog();
protected:
    const QStringList m_sl;
};

}

#endif /* _confgui_h_included_ */
