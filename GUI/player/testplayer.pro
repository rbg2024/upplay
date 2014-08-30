TEMPLATE        = app
LANGUAGE        = C++

QT += webkit

QMAKE_CXXFLAGS += -std=c++0x 

CONFIG  += qt warn_on thread release 

HEADERS += GUI_Player.h GUI_TrayIcon.h SearchSlider.h

SOURCES += \
        GUI_PlayerConnections.cpp \
        GUI_PlayerControls.cpp \
        GUI_PlayerCover.cpp \
        GUI_Player.cpp \
        GUI_PlayerMenubar.cpp \
        GUI_PlayerPlugins.cpp \
        GUI_TrayIcon.cpp \
        testmain.cpp \
        SearchSlider.cpp \
        ../../HelperStructs/Helper.cpp

FORMS   = GUI_Player.ui

RESOURCES = 

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  INCLUDEPATH += ../../
}
