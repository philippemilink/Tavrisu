QT += \
    widgets \
    sql \
    xml \
    network

HEADERS += \
    ViewPluginInterface.h \
    SelectView.h \
    QSqliteCom.h \
    QlibVLC.h \
    PluginLoader.h \
    NewMusic.h \
    NewArtist.h \
    MainWindow.h \
    FirstUse.h \
    ButtonImage.h \
    AboutDialog.h \
    InitializeDatabase.h \
    InitSelectAction.h \
    InitImportXML.h \
    ExportXML.h \
    InitAnalyzeSelect.h \
    InitAnalyzeArtist.h \
    AlbumWidget.h \
    AlbumDialog.h \
    InitAnalyzeAlbum.h \
    Updater.h \
    Settings.h

SOURCES += \
    SelectView.cpp \
    QSqliteCom.cpp \
    QlibVLC.cpp \
    NewMusic.cpp \
    NewArtist.cpp \
    MainWindow.cpp \
    main.cpp \
    FirstUse.cpp \
    ButtonImage.cpp \
    AboutDialog.cpp \
    InitializeDatabase.cpp \
    InitSelectAction.cpp \
    InitImportXML.cpp \
    ExportXML.cpp \
    InitAnalyzeSelect.cpp \
    InitAnalyzeArtist.cpp \
    AlbumWidget.cpp \
    AlbumDialog.cpp \
    InitAnalyzeAlbum.cpp \
    Updater.cpp \
    Settings.cpp

FORMS += \
    SelectView.ui \
    NewMusic.ui \
    NewArtist.ui \
    MainWindow.ui \
    AboutDialog.ui \
    InitSelectAction.ui \
    InitImportXML.ui \
    ExportXML.ui \
    InitAnalyzeSelect.ui \
    InitAnalyzeArtist.ui \
    AlbumWidget.ui \
    InitAnalyzeAlbum.ui \
    Updater.ui \
    Settings.ui

win32: LIBS += -L$$PWD/../../../../../../../Qt/Qt5.0.2/5.0.2/mingw47_32/lib/ -llibvlc

INCLUDEPATH += $$PWD/../../../../../../../Qt/Qt5.0.2/5.0.2/mingw47_32/include
DEPENDPATH += $$PWD/../../../../../../../Qt/Qt5.0.2/5.0.2/mingw47_32/include

unix:!macx: LIBS += -L$$PWD/../../../../../../../usr/lib/ -lvlc

INCLUDEPATH += $$PWD/../../../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../../../usr/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../../../usr/lib/libvlc.a
