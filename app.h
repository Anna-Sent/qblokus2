#ifndef APP_H
#define APP_H

#include "client.h"
#include "game.h"
#include "server.h"
#include "serversearcher.h"
#include "ui_racingForm.h"
#include <QThread>

class App : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

private:
    Game *_game;
    LocalClient _localClient;
    QThread _localClientThread;
    Server _server;
    ServerSearcher _serverSearcher;
    QThread _serverThread;
    bool confirm(const QString &) const;
    void setTabOrder() const;
    void showCriticalMessage(const QString &) const;
    void showInformationMessage(const QString &) const;
    void showWarningMessage(const QString &) const;

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
    void completeTurn(const ClientInfo &, const QString &, int, int, int);
    void finishGame(QList<ClientInfo>, int);
    void startTurn(const ClientInfo &);

    // from server
    void processServerStarted();

public:
    App(QWidget *parent = 0);

signals:
    // to server
    void gameStarted();
    void gameStopped();
    void readyToStartServer(int, quint16);
    void readyToStopServer();

    // to local client
    void chatMessagePosted(const ClientInfo &, const QString &);
    void readyToStartLocalClient(const QString &hostname, quint16 port);
    void readyToStopLocalClient();
    void surrendered(const ClientInfo &);
};

#endif
