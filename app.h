#ifndef APP_H
#define APP_H

#include "ui_racingForm.h"
#include "game.h"
#include "client.h"
#include "server.h"
#include "clientinfo.h"
#include "messagerecv.h"
#include "serverssearcher.h"

class App : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT
public:
    App(QWidget *parent = 0);
    ~App();
private:
    QMap<QString, QList<ClientInfo> > servers;
    Server server;
    LocalClient localClient;
    ServersSearcher serversSearcher;
    void perror(QString);
    void pinfo(QString);
public:
    Game *game;
public slots:
    // from graphics
    void a_startGame();
    void le_sendMessage();
    void a_exit();
    void a_disconnectFromServer();
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

    void connectBtnClicked();
    void searchBtnClicked();
    void toggled(bool);
    void getServer(QString, QList<ClientInfo>);
    void itemClicked ( QListWidgetItem * item );
signals:
    // to server
    void startGame();
    void restartGame(QList<ClientInfo>);
    // to client
    void sendMessage(QString);
    void tolcTurnDone(QString name,QColor color,QString tile,int id,int x,int y);
    void tolcPlayerSurrendered(QString name,QColor color);
};

#endif
