#ifndef APP_H
#define APP_H

#include "ui_racingForm.h"
#include "game.h"
#include "client.h"
#include "server.h"
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
    void localChatMessageReceive(QString, QColor, QString);
    void localPlayersListMessageReceive(QList<ClientInfo>);
    void localClientConnectMessageReceive(QString, QColor);
    void localClientDisconnectMessageReceive(QString, QColor);
    void localConnectionAcceptedMessageReceive(int);

    // game signals
    void localTurnMessageReceive(QColor, int, int, int, QString);
    void localStartGameMessageReceive();
    void localRestartGameMessageReceive(QList<ClientInfo>);
    void localSurrenderMessageReceive(QString, QColor);

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
