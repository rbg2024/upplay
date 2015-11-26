TEMPLATE        = app
LANGUAGE        = C++

QMAKE_CXXFLAGS += -std=c++0x 

CONFIG  += qt warn_on thread release 
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS += volumewidgetif.h volumewidget.h ../widgets/DirectSlider.h \
        ../../HelperStructs/CSettingsStorage.h

SOURCES += vw.cpp volumewidget.cpp \
        ../widgets/DirectSlider.cpp \
        ../../HelperStructs/Helper.cpp \
        ../../HelperStructs/CSettingsStorage.cpp 
        
# FORMS   = volumewidget.ui

RESOURCES = ../upplay.qrc

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  INCLUDEPATH += ../../
}
