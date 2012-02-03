#ifndef APP_H
#define APP_H

#include "client.h"
#include "game.h"
#include "server.h"
#include "serverssearcher.h"
#include "ui_racingForm.h"

class App : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT

public:
    App(QWidget *parent = 0);

private:
    Game *_game;
    LocalClient _localClient;
    Server _server;
    ServersSearcher _serversSearcher;
    void setTabOrder();

private slots:
    void perror(const QString &);
    void pinfo(const QString &);

    // from user
    void userDisconnectFromServer();
    void userQuit();
    void userSendMessage();
    void userStartGame();
    void userTryToConnect();

    void guiCreateServerToggled(bool);
    void guiPortValueChanged(int);
    void guiServersListItemClicked(QListWidgetItem *item);
    void guiServersListCurrentTextChanged(QString);

    // from local client
    void chatMessageReceived(QString, QColor, QString);
    void clientConnectMessageReceived(QString, QColor);
    void clientDisconnected();
    void clientDisconnectMessageReceived(QString, QColor);
    void connectionAccepted();
    void playersListMessageReceived(QList<ClientInfo>);
    void startGameMessageReceived(QList<ClientInfo>);
    void surrenderMessageReceived(QString, QColor);
    void turnMessageReceived(QColor, int, int, int, QString);

    // from servers searcher
    void serverInfoMessageReceive(const QHostAddress &, QList<ClientInfo>);

    // from game
    void gameOver(QString, int, QColor);

signals:
    // to server
    void gameStarted();
    void gameRestarted(QList<ClientInfo>);

    // to client
    void messageSent(QString);
    void playerSurrendered(QString name, QColor color);
    void turnDone(QString name, QColor color, QString tile, int id, int x, int y);
};

#endif
