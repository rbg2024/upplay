TEMPLATE        = app
LANGUAGE        = C++

QT += webkit

QMAKE_CXXFLAGS += -std=c++0x 

CONFIG  += qt warn_on thread release 

HEADERS += \
        GUI/main/mainw.h \
        GUI/main/trayicon.h \
        GUI/playerwidget/playerwidget.h \
        GUI/mdatawidget/mdatawidget.h \
        GUI/mdatawidget/mdatawidgetif.h \
        GUI/volumewidget/volumewidget.h \
        GUI/volumewidget/volumewidgetif.h \
        GUI/playctlwidget/playctlwidget.h \
        GUI/playctlwidget/playctlwidgetif.h \
        GUI/progresswidget/progresswidget.h \
        GUI/progresswidget/progresswidgetif.h \
        GUI/widgets/directslider.h \
        HelperStructs/CSettingsStorage.h

SOURCES += \
        testmain.cpp \
        GUI/main/mainw.cpp \
        GUI/main/mw_connections.cpp  \
        GUI/main/mw_controls.cpp  \
        GUI/main/mw_cover.cpp  \
        GUI/main/mw_menubar.cpp  \
        GUI/main/trayicon.cpp \
        GUI/mdatawidget/mdatawidget.cpp \
        GUI/volumewidget/volumewidget.cpp \
        GUI/playctlwidget/playctlwidget.cpp \
        GUI/progresswidget/progresswidget.cpp \
        GUI/widgets/directslider.cpp \
        HelperStructs/Helper.cpp \
        HelperStructs/CSettingsStorage.cpp

FORMS   = \
        GUI/main/mainw.ui \
        GUI/playerwidget/playerwidget.ui \
        GUI/mdatawidget/mdatawidget.ui \
        GUI/volumewidget/volumewidget.ui \
        GUI/playctlwidget/playctlwidget.ui \
        GUI/progresswidget/progresswidget.ui

RESOURCES = GUI/upplay.qrc

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  INCLUDEPATH += ../../
}
