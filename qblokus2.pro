CONFIG      +=  console qt thread

SOURCES     =   main.cpp \
                app.cpp \
                clientinfo.cpp \
                localclient.cpp \
                message.cpp \
                messagereceiver.cpp \
                remoteclient.cpp \
                server.cpp \
                serversearcher.cpp \
                translationservice.cpp

HEADERS     =   app.h \
                constants.h \
                clientinfo.h \
                localclient.h \
                message.h \
                messagereceiver.h \
                remoteclient.h \
                server.h \
                serversearcher.h \
                translationservice.h

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

RESOURCES   +=  qblokus2.qrc

TRANSLATIONS+=  qblokus2_ru.ts

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

isEmpty(QMAKE_QM_FILE) {
    win32:QMAKE_QM_FILE = ${QMAKE_FILE_PATH}\${QMAKE_FILE_BASE}.qm
    else:QMAKE_QM_FILE = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
}

QMAKE_EXTRA_COMPILERS   +=  lrelease
lrelease.input          =   TRANSLATIONS
lrelease.output         =   $$QMAKE_QM_FILE
lrelease.commands       =   $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $$QMAKE_QM_FILE
lrelease.CONFIG         +=  no_link target_predeps
