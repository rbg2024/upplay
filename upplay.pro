TEMPLATE        = app
LANGUAGE        = C++

QT += webkit

QMAKE_CXXFLAGS += -std=c++0x 

CONFIG  += qt warn_on thread release 

HEADERS += \
        dirreader.h \
        mainw.h \
        cdbrowser.h \

SOURCES += \
        upplay.cpp \
        cdbrowser.cpp

FORMS   = \
        mainw.ui

RESOURCES = 

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  LIBS += -lupnpp
}
