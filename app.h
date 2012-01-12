#ifndef APP_H
#define APP_H

#include "ui_racingForm.h"
#include "game.h"
#include "client.h"
#include "server.h"
#include "messagereceiver.h"
#include "serverssearcher.h"

class App : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT

public:
    App(QWidget *parent = 0);
    ~App();

private:
    Game *game;
    Server server;
    LocalClient localClient;
    ServersSearcher serversSearcher;
    void perror(QString);
    void pinfo(QString);

public slots:
    // from graphics
    void userStartGame();
    void userQuit();
    void userDisconnectFromServer();
    void userSendMessage();

    // from local client
    void localError(QString);
    void localConnected();
    void localDisconnected();
    void localChatMessageReceive(ChatMessage);
    void localPlayersListMessageReceive(PlayersListMessage);
    void localClientConnectMessageReceive(ClientConnectMessage);
    void localClientDisconnectMessageReceive(ClientDisconnectMessage);
    void localConnectionAcceptedMessageReceive(ConnectionAcceptedMessage);

    // game signals
    void localTurnMessageReceive(TurnMessage);
    void localStartGameMessageReceive(StartGameMessage);
    void localRestartGameMessageReceive(RestartGameMessage);
    void localSurrenderMessageReceive(SurrenderMessage);

    void connectClicked();
    void serversListItemClicked(QListWidgetItem *item);
    void serversListCurrentTextChanged(QString);
    void createServerToggled(bool);

    // from servers searcher
    void serverInfoMessageReceive(QString, QList<ClientInfo>);

signals:
    // to server
    void startGame();
    void restartGame(QList<ClientInfo>);

    // to client
    void sendMessage(QString);
    void turnDone(QString name, QColor color, QString tile, int id, int x, int y);
    void playerSurrendered(QString name, QColor color);
};

#endif
