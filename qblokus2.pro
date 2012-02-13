CONFIG += console qt thread
SOURCES = main.cpp \
    app.cpp \
    messagereceiver.cpp \
    clientinfo.cpp \
    client.cpp \
    server.cpp \
    serversearcher.cpp \
    message.cpp
HEADERS = app.h \
    messagereceiver.h \
    clientinfo.h \
    client.h \
    server.h \
    serversearcher.h \
    constants.h \
    message.h
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

FORMS += racingForm.ui
