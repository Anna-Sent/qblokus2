CONFIG += console qt thread
SOURCES = main.cpp \
    app.cpp \
    messagerecv.cpp \
    clientinfo.cpp \
    client.cpp \
    server.cpp
HEADERS = app.h \
    messagerecv.h \
    clientinfo.h \
    client.h \
    server.h
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

FORMS += racingForm.ui \
    optionsDialog.ui
