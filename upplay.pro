TEMPLATE        = app
LANGUAGE        = C++

QT += webkit

QMAKE_CXXFLAGS += -std=c++0x 

CONFIG  += qt warn_on thread release 

HEADERS += \
        application.h \
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
        HelperStructs/CSettingsStorage.h \
        HelperStructs/Helper.h \
        HelperStructs/PlaylistMode.h \
        HelperStructs/Style.h \
        playlist/Playlist.h \
        \
        upqo/cdirectory_qo.h \
        cdbrowser.h

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
        application.cpp \
        HelperStructs/CSettingsStorage.cpp \
        HelperStructs/Helper.cpp \
        HelperStructs/Style.cpp \
        playlist/Playlist_GUI_slots.cpp \
        playlist/Playlist.cpp \
        cdbrowser.cpp \
        upplay.cpp \
        upputils.cpp

FORMS   = \
        GUI/player/GUI_Player.ui \
        GUI/playlist/GUI_Playlist.ui \
        GUI/playlist/entry/GUI_PlaylistEntryBig.ui \
        GUI/playlist/entry/GUI_PlaylistEntrySmall.ui 

RESOURCES = 

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  LIBS += -lupnpp
  INCLUDEPATH += contentDirectory/
}
