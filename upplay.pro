TEMPLATE        = app
LANGUAGE        = C++

QT += webkit

QMAKE_CXXFLAGS += -std=c++0x  -DUPPLAY_VERSION=\\\"0.1.0\\\"

CONFIG  += qt warn_on thread release debug

RESOURCES += GUI/upplay.qrc

HEADERS += \
        GUI/ContextMenu.h \
        GUI/player/GUI_Player.h \
        GUI/player/GUI_TrayIcon.h \
        GUI/player/SearchSlider.h  \
        GUI/playlist/GUI_Playlist.h \
        GUI/playlist/delegate/PlaylistItemDelegate.h \
        GUI/playlist/delegate/PlaylistItemDelegateInterface.h \
        GUI/playlist/entry/GUI_PlaylistEntry.h \
        GUI/playlist/entry/GUI_PlaylistEntryBig.h \
        GUI/playlist/entry/GUI_PlaylistEntrySmall.h \
        GUI/playlist/model/PlaylistItemModel.h \
        GUI/playlist/view/PlaylistView.h \
        GUI/renderchoose/renderchoose.h \
        HelperStructs/CSettingsStorage.h \
        HelperStructs/Helper.h \
        HelperStructs/PlaylistMode.h \
        HelperStructs/Style.h \
        application.h \
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
        upqo/renderingcontrol_qo.h

SOURCES += \
        GUI/ContextMenu.cpp \
        GUI/player/GUI_Player.cpp \
        GUI/player/GUI_PlayerConnections.cpp \
        GUI/player/GUI_PlayerControls.cpp \
        GUI/player/GUI_PlayerCover.cpp \
        GUI/player/GUI_PlayerMenubar.cpp \
        GUI/player/GUI_PlayerPlugins.cpp \
        GUI/player/GUI_TrayIcon.cpp \
        GUI/player/SearchSlider.cpp \
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
        dirbrowser/cdbrowser.cpp \
        playlist/Playlist.cpp \
        playlist/PlaylistAVT.cpp \
        playlist/PlaylistOH.cpp \
        upadapt/upputils.cpp \
        upplay.cpp 

FORMS   = \
        GUI/player/GUI_Player.ui \
        GUI/playlist/GUI_Playlist.ui \
        GUI/playlist/entry/GUI_PlaylistEntryBig.ui \
        GUI/playlist/entry/GUI_PlaylistEntrySmall.ui \
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


  target.path = "$$PREFIX/bin"

  data.files = dirbrowser/cdbrowser.css
  data.path = $$PREFIX/share/upplay/cdbrowser

  INSTALLS += target data
}
