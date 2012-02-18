CONFIG      +=  console qt thread

SOURCES     =   main.cpp \
                app.cpp \
                clientinfo.cpp \
                localclient.cpp \
                remoteclient.cpp \
                server.cpp \
                serversearcher.cpp \
                message.cpp \
                messagereceiver.cpp

HEADERS     =   app.h \
                constants.h \
                clientinfo.h \
                localclient.h \
                message.h \
                messagereceiver.h \
                remoteclient.h \
                server.h \
                serversearcher.h

SOURCES     +=  coloritem.cpp \
                tile.cpp \
                table.cpp \
                player.cpp \
                localplayer.cpp \
                networkplayer.cpp \
                game.cpp

HEADERS     +=  coloritem.h \
                tile.h \
                table.h \
                player.h \
                localplayer.h \
                networkplayer.h \
                game.h

FORMS       +=  racingForm.ui

QT          +=  network

RESOURCES   =  qblokus2.qrc

TRANSLATIONS+=  qblokus2_ru.ts
