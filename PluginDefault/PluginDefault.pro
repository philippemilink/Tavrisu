QT += \
    widgets \
    sql

TEMPLATE = lib
CONFIG += plugin
TARGET = Default

SOURCES += \
    Player.cpp \
    Default.cpp \
    ../Core/QSqliteCom.cpp

FORMS += \
    Player.ui

HEADERS += \
    Player.h \
    Default.h \
    ../Core/ViewPluginInterface.h \
    ../Core/QSqliteCom.h


