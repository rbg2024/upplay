TEMPLATE        = app
LANGUAGE        = C++

QMAKE_CXXFLAGS += -std=c++0x 

CONFIG  += qt warn_on thread release 

HEADERS += \
        ../playerwidget/playerwidget.h \
        ../mdatawidget/mdatawidget.h \
        ../mdatawidget/mdatawidgetif.h \
        ../volumewidget/volumewidget.h \
        ../volumewidget/volumewidgetif.h \
        ../playctlwidget/playctlwidget.h \
        ../playctlwidget/playctlwidgetif.h \
        ../progresswidget/progresswidget.h \
        ../progresswidget/progresswidgetif.h \
        ../widgets/directslider.h \
        ../../HelperStructs/CSettingsStorage.h

SOURCES += plyr.cpp \
        ../mdatawidget/mdatawidget.cpp \
        ../playerwidget/playerwidget.cpp \
        ../volumewidget/volumewidget.cpp \
        ../playctlwidget/playctlwidget.cpp \
        ../progresswidget/progresswidget.cpp \
        ../widgets/directslider.cpp \
        ../../HelperStructs/Helper.cpp \
        ../../HelperStructs/CSettingsStorage.cpp
        
FORMS   = \
        ../playerwidget/playerwidget.ui \
        ../mdatawidget/mdatawidget.ui \
        ../volumewidget/volumewidget.ui \
        ../playctlwidget/playctlwidget.ui \
        ../progresswidget/progresswidget.ui

RESOURCES = ../upplay.qrc

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  INCLUDEPATH += ../.. ../mdatawidget ../playctlwidget ../volumewidget \
              ../progresswidget
}
