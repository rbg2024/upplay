/********************************************************************************
** Form generated from reading UI file 'dirbrowser.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIRBROWSER_H
#define UI_DIRBROWSER_H

#include <QtCore/QVariant>
#include <QtWebKitWidgets/QWebView>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "dirbrowser/cdbrowser.h"

QT_BEGIN_NAMESPACE

class Ui_DirBrowser
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabs;
    QWidget *tab;
    QVBoxLayout *verticalLayout_3;
    CDBrowser *cdBrowser;
    QFrame *searchFrame;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QComboBox *searchCMB;
    QPushButton *execSearchPB;
    QToolButton *prevTB;
    QToolButton *nextTB;
    QCheckBox *serverSearchCB;
    QComboBox *propsCMB;
    QSpacerItem *horizontalSpacer;
    QToolButton *closeSearchTB;

    void setupUi(QWidget *DirBrowser)
    {
        if (DirBrowser->objectName().isEmpty())
            DirBrowser->setObjectName(QStringLiteral("DirBrowser"));
        DirBrowser->resize(556, 597);
        verticalLayout_2 = new QVBoxLayout(DirBrowser);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(4);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabs = new QTabWidget(DirBrowser);
        tabs->setObjectName(QStringLiteral("tabs"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_3 = new QVBoxLayout(tab);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        cdBrowser = new CDBrowser(tab);
        cdBrowser->setObjectName(QStringLiteral("cdBrowser"));
        cdBrowser->setUrl(QUrl(QStringLiteral("about:blank")));

        verticalLayout_3->addWidget(cdBrowser);

        tabs->addTab(tab, QString());

        verticalLayout->addWidget(tabs);

        searchFrame = new QFrame(DirBrowser);
        searchFrame->setObjectName(QStringLiteral("searchFrame"));
        searchFrame->setFrameShape(QFrame::StyledPanel);
        searchFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(searchFrame);
        horizontalLayout_2->setSpacing(4);
        horizontalLayout_2->setContentsMargins(2, 2, 2, 2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        searchCMB = new QComboBox(searchFrame);
        searchCMB->setObjectName(QStringLiteral("searchCMB"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(searchCMB->sizePolicy().hasHeightForWidth());
        searchCMB->setSizePolicy(sizePolicy);
        searchCMB->setMinimumSize(QSize(150, 0));
        searchCMB->setMaximumSize(QSize(300, 16777215));
        searchCMB->setEditable(true);

        horizontalLayout->addWidget(searchCMB);

        execSearchPB = new QPushButton(searchFrame);
        execSearchPB->setObjectName(QStringLiteral("execSearchPB"));
        execSearchPB->setEnabled(false);
        execSearchPB->setMaximumSize(QSize(70, 16777215));

        horizontalLayout->addWidget(execSearchPB);

        prevTB = new QToolButton(searchFrame);
        prevTB->setObjectName(QStringLiteral("prevTB"));
        prevTB->setIconSize(QSize(27, 20));
        prevTB->setArrowType(Qt::LeftArrow);

        horizontalLayout->addWidget(prevTB);

        nextTB = new QToolButton(searchFrame);
        nextTB->setObjectName(QStringLiteral("nextTB"));
        nextTB->setIconSize(QSize(27, 20));
        nextTB->setArrowType(Qt::RightArrow);

        horizontalLayout->addWidget(nextTB);

        serverSearchCB = new QCheckBox(searchFrame);
        serverSearchCB->setObjectName(QStringLiteral("serverSearchCB"));

        horizontalLayout->addWidget(serverSearchCB);

        propsCMB = new QComboBox(searchFrame);
        propsCMB->setObjectName(QStringLiteral("propsCMB"));

        horizontalLayout->addWidget(propsCMB);

        horizontalSpacer = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        closeSearchTB = new QToolButton(searchFrame);
        closeSearchTB->setObjectName(QStringLiteral("closeSearchTB"));
        closeSearchTB->setIconSize(QSize(20, 20));
        closeSearchTB->setAutoRaise(true);

        horizontalLayout->addWidget(closeSearchTB);


        horizontalLayout_2->addLayout(horizontalLayout);


        verticalLayout->addWidget(searchFrame);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(DirBrowser);

        tabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DirBrowser);
    } // setupUi

    void retranslateUi(QWidget *DirBrowser)
    {
        DirBrowser->setWindowTitle(QApplication::translate("DirBrowser", "Form", 0));
        tabs->setTabText(tabs->indexOf(tab), QApplication::translate("DirBrowser", "Servers", 0));
        execSearchPB->setText(QApplication::translate("DirBrowser", "Search", 0));
        prevTB->setText(QApplication::translate("DirBrowser", "...", 0));
        nextTB->setText(QApplication::translate("DirBrowser", "...", 0));
        serverSearchCB->setText(QApplication::translate("DirBrowser", "Server Search", 0));
        closeSearchTB->setText(QApplication::translate("DirBrowser", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class DirBrowser: public Ui_DirBrowser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIRBROWSER_H
