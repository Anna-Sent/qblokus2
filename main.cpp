#include "app.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    //qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<ChatMessage>("ChatMessage");
    qRegisterMetaType<ClientConnectMessage>("ClientConnectMessage");
    qRegisterMetaType<ClientDisconnectMessage>("ClientDisconnectMessage");
    qRegisterMetaType<ConnectionAcceptedMessage>("ConnectionAcceptedMessage");
    qRegisterMetaType<PingMessage>("PingMessage");
    qRegisterMetaType<PlayersListMessage>("PlayersListMessage");
    qRegisterMetaType<RestartGameMessage>("RestartGameMessage");
    qRegisterMetaType<ServerInfoMessage>("ServerInfoMessage");
    qRegisterMetaType<ServerReadyMessage>("ServerReadyMessage");
    qRegisterMetaType<ServerRequestMessage>("ServerRequestMessage");
    qRegisterMetaType<StartGameMessage>("StartGameMessage");
    qRegisterMetaType<TryToConnectMessage>("TryToConnectMessage");
    qRegisterMetaType<TurnMessage>("TurnMessage");
    qRegisterMetaType<SurrenderMessage>("SurrenderMessage");
    qRegisterMetaTypeStreamOperators<ClientInfo>("ClientInfo");
    qRegisterMetaTypeStreamOperators<QList<ClientInfo> >("QList<ClientInfo>");

    QApplication app(argc, argv);
    App *dialog = new App;
    dialog->show();
    return app.exec();
}
