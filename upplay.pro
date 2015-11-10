TEMPLATE        = app
LANGUAGE        = C++

QT += script webkit network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

# VERSION is ^egrepped and must stay in the first column
VERSION = 0.10.1

QMAKE_CXXFLAGS += -DUPPLAY_VERSION=\\\"$$VERSION\\\"
QMAKE_CXXFLAGS += -std=c++0x


CONFIG  += qt warn_on thread debug

RESOURCES += GUI/upplay.qrc

HEADERS += \
        GUI/ContextMenu.h \
        GUI/player/GUI_Player.h \
        GUI/player/GUI_TrayIcon.h \
        GUI/player/DirectSlider.h  \
        GUI/playlist/GUI_Playlist.h \
        GUI/playlist/delegate/PlaylistItemDelegate.h \
        GUI/playlist/delegate/PlaylistItemDelegateInterface.h \
        GUI/playlist/entry/GUI_PlaylistEntry.h \
        GUI/playlist/entry/GUI_PlaylistEntryBig.h \
        GUI/playlist/entry/GUI_PlaylistEntrySmall.h \
        GUI/playlist/model/PlaylistItemModel.h \
        GUI/playlist/view/PlaylistView.h \
        GUI/prefs/sortprefs.h \
        GUI/renderchoose/renderchoose.h \
        HelperStructs/CSettingsStorage.h \
        HelperStructs/Helper.h \
        HelperStructs/PlaylistMode.h \
        HelperStructs/Style.h \
        application.h \
        dirbrowser/dirbrowser.h \
        dirbrowser/cdbrowser.h \
        dirbrowser/rreaper.h \
        playlist/Playlist.h \
        playlist/PlaylistAVT.h \
        playlist/PlaylistOH.h \
        upadapt/avtadapt.h \
        upadapt/ohpladapt.h \
        upqo/avtransport_qo.h \
        upqo/cdirectory_qo.h \
        upqo/ohplaylist_qo.h \
        upqo/ohtime_qo.h \
        upqo/ohvolume_qo.h \
        upqo/renderingcontrol_qo.h

SOURCES += \
        GUI/ContextMenu.cpp \
        GUI/player/GUI_Player.cpp \
        GUI/player/GUI_PlayerConnections.cpp \
        GUI/player/GUI_PlayerControls.cpp \
        GUI/player/GUI_PlayerCover.cpp \
        GUI/player/GUI_PlayerMenubar.cpp \
        GUI/player/GUI_TrayIcon.cpp \
        GUI/player/DirectSlider.cpp \
        GUI/playlist/GUI_Playlist.cpp \
        GUI/playlist/delegate/PlaylistItemDelegate.cpp \
        GUI/playlist/entry/GUI_PlaylistEntryBig.cpp \
        GUI/playlist/entry/GUI_PlaylistEntrySmall.cpp \
        GUI/playlist/model/PlaylistItemModel.cpp \
        GUI/playlist/view/PlaylistView.cpp \
        HelperStructs/CSettingsStorage.cpp \
        HelperStructs/Helper.cpp \
        HelperStructs/Style.cpp \
        application.cpp \
        dirbrowser/dirbrowser.cpp \
        dirbrowser/cdbrowser.cpp \
        playlist/Playlist.cpp \
        playlist/PlaylistAVT.cpp \
        playlist/PlaylistOH.cpp \
        upadapt/upputils.cpp \
        upplay.cpp 

FORMS   = \
        dirbrowser/dirbrowser.ui \
        GUI/player/GUI_Player.ui \
        GUI/playlist/GUI_Playlist.ui \
        GUI/playlist/entry/GUI_PlaylistEntryBig.ui \
        GUI/playlist/entry/GUI_PlaylistEntrySmall.ui \
        GUI/prefs/sortprefs.ui \
        GUI/renderchoose/renderchoose.ui

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  LIBS += -lupnpp
  isEmpty(PREFIX) {
    PREFIX = /usr
  }
  message("Prefix is $$PREFIX")
  DEFINES += PREFIX=\\\"$$PREFIX\\\"

 # Installation stuff
  target.path = "$$PREFIX/bin"

  bdata.files = dirbrowser/cdbrowser.css dirbrowser/dark.css \
              dirbrowser/standard.css
  bdata.path = $$PREFIX/share/upplay/cdbrowser   
  gdata.files = GUI/standard.css GUI/dark.css
  gdata.path = $$PREFIX/share/upplay/
  desktop.files += upplay.desktop
  desktop.path = $$PREFIX/share/applications/
  icona.files += GUI/icons/upplay.png
  icona.path = $$PREFIX/share/icons/hicolor/48x48/apps/
  iconb.files += GUI/icons/upplay.png
  iconb.path = $$PREFIX/share/pixmaps/
  INSTALLS += target bdata desktop gdata icona iconb
}

windows {
    contains(QMAKE_CC, gcc){
        # MingW
        QMAKE_CXXFLAGS += -std=c++11 -Wno-unused-parameter
    }
    contains(QMAKE_CC, cl){
        # Visual Studio
    }
  DEFINES += WIN32
  DEFINES -= UNICODE
  DEFINES -= _UNICODE
  DEFINES += _MBCS
  DEFINES += PSAPI_VERSION=1
  INCLUDEPATH += c:/users/bill/documents/upnp/libupnpp
  INCLUDEPATH += c:/users/bill/documents/upnp/pupnp/include
  LIBS += -Lc:/Users/Bill/Documents/UPnP/libupnpp/windows/build-libupnpp-Desktop_Qt_5_5_0_MinGW_32bit-Debug/debug -lupnpp
  LIBS += c:/users/bill/documents/upnp/expat-2.1.0/.libs/libexpat.a
  LIBS += c:/users/bill/documents/upnp/curl-7.43.0/lib/libcurl.a
  LIBS += c:/users/bill/documents/upnp/pupnp/upnp/.libs/libupnp.a
  LIBS += c:/users/bill/documents/upnp/pupnp/ixml/.libs/libixml.a
  LIBS += c:/users/bill/documents/upnp/pupnp/threadutil/.libs/libthreadutil.a
  LIBS += -liphlpapi
  LIBS += -lwldap32
  LIBS += -lws2_32
  LIBS += -lshlwapi
}
