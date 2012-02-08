#ifndef APP_H
#define APP_H

#include "client.h"
#include "game.h"
#include "server.h"
#include "serverssearcher.h"
#include "ui_racingForm.h"

class App : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

private:
    Game *_game;
    LocalClient _localClient;
    Server _server;
    ServersSearcher _serversSearcher;
    void setTabOrder();

private slots:
    void perror(const QString &);
    void pinfo(const QString &, const QColor &color = Qt::darkBlue);

    // from user
    void userDisconnectFromServer();
    void userQuit();
    void userSendMessage();
    void userStartGame();
    void userTryToConnect();

    void guiChangePortValue(int);
    void guiChangeServersListCurrentText(QString);
    void guiClickRetirePlayer();
    void guiClickServersListItem(QListWidgetItem *item);
    void guiToggleCreateServer(bool);

    // from local client
    void acceptConnection();
    void processClientDisconnected();
    void receiveChatMessage(QString, QColor, QString);
    void receiveClientConnectMessage(QString, QColor);
    void receiveClientDisconnectMessage(QString, QColor);
    void receivePlayersListMessage(QList<ClientInfo>);
    void receiveStartGameMessage(QList<ClientInfo>);
    void receiveSurrenderMessage(QString, QColor);
    void receiveTurnMessage(QColor, int, int, int, QString);

    // from servers searcher
    void receiveServerInfoMessage(const QHostAddress &, QList<ClientInfo>);

    // from game
    void completeTurn(QString, QColor, QString, int, int, int);
    void finishGame(QList<ClientInfo>, int);
    void startTurn(const ClientInfo &);

public:
    App(QWidget *parent = 0);
};

#endif
