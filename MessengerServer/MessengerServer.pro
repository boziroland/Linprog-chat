QT += core network gui widgets sql

TARGET = MessengerServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += debug_and_release

HEADERS += \
    chatserver.h \
    serverworker.h \
    serverwindow.h \
    msg.h \
    serverdatabase.h

SOURCES += \
    chatserver.cpp \
    servermain.cpp \
    serverworker.cpp \
    serverwindow.cpp \
    serverdatabase.cpp

FORMS += \
    serverwindow.ui
