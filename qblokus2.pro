CONFIG += console qt thread
SOURCES = main.cpp \
    app.cpp \
    messagereceiver.cpp \
    clientinfo.cpp \
    remoteclient.cpp \
    server.cpp \
    serversearcher.cpp \
    message.cpp \
    localclient.cpp
HEADERS = app.h \
    messagereceiver.h \
    clientinfo.h \
    remoteclient.h \
    server.h \
    serversearcher.h \
    constants.h \
    message.h \
    localclient.h
SOURCES += coloritem.cpp \
    tile.cpp \
    table.cpp \
    player.cpp \
    localplayer.cpp \
    game.cpp \
    networkplayer.cpp
HEADERS += coloritem.h \
    tile.h \
    table.h \
    player.h \
    localplayer.h \
    game.h \
    networkplayer.h
QT += network
TRANSLATIONS += qblokus2_ru.ts
FORMS += racingForm.ui
