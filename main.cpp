#include "app.h"
#include <QApplication>

#define     ERROR_INCORRECT_PLAYERS_COUNT   1
#define     ERROR_INCORRECT_PORT_VALUE      2
#define     ERROR_INCORRECT_CONNECTION      3
#define     ERROR_INCORRECT_ARGUMENTS       4

void usage()
{
    char *app = qApp->argv()[0];
    QTextStream qout(stdout);
    qout << QString::fromUtf8("Usage:") << endl;
    qout << app << QString::fromUtf8(" -- run GUI application") << endl;
    qout << app << QString::fromUtf8(" -s [3|4] [PORT] -- run non-GUI game server listening on the specified port, for 3 or 4 players") << endl;
    qout << app << QString::fromUtf8(" -u -- show usage and exit") << endl;
}

void error(int code)
{
    QTextStream qerr(stderr);
    switch (code)
    {
    case ERROR_INCORRECT_PLAYERS_COUNT:
        qerr << QString::fromUtf8("Incorrect players count. Players count must be equal 3 or 4.") << endl;
        break;
    case ERROR_INCORRECT_PORT_VALUE:
        qerr << QString::fromUtf8("Incorrect port value. Port value must be in the range from 1 to 65535.") << endl;
        break;
    case ERROR_INCORRECT_CONNECTION:
        qerr << QString::fromUtf8("Can't start server") << endl;
        break;
    case ERROR_INCORRECT_ARGUMENTS:
        qerr << QString::fromUtf8("Incorrect command line arguments") << endl;
        break;
    default:
        if (code != 0)
        {
            qerr << QString::fromUtf8("Unknown error occured") << endl;
        }
    };
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<ChatMessage>("ChatMessage");
    qRegisterMetaType<ClientConnectMessage>("ClientConnectMessage");
    qRegisterMetaType<ClientDisconnectMessage>("ClientDisconnectMessage");
    qRegisterMetaType<ConnectionAcceptedMessage>("ConnectionAcceptedMessage");
    qRegisterMetaType<PingMessage>("PingMessage");
    qRegisterMetaType<PlayersListMessage>("PlayersListMessage");
    qRegisterMetaType<StartGameMessage>("StartGameMessage");
    qRegisterMetaType<ServerInfoMessage>("ServerInfoMessage");
    qRegisterMetaType<ServerReadyMessage>("ServerReadyMessage");
    qRegisterMetaType<ServerRequestMessage>("ServerRequestMessage");
    qRegisterMetaType<TryToConnectMessage>("TryToConnectMessage");
    qRegisterMetaType<TurnMessage>("TurnMessage");
    qRegisterMetaType<SurrenderMessage>("SurrenderMessage");
    qRegisterMetaTypeStreamOperators<ClientInfo>("ClientInfo");
    qRegisterMetaTypeStreamOperators<QList<ClientInfo> >("QList<ClientInfo>");

    QApplication app(argc, argv);
    if (argc == 1)
    {
        App *dialog = new App;
        dialog->show();
        return app.exec();
    }
    else if (argc == 2 && strcmp(argv[1], "-u") == 0)
    {
        usage();
        return 0;
    }
    else if (argc == 4 && strcmp(argv[1], "-s") == 0)
    {
        QStringList arguments = app.arguments();
        bool ok;

        int playersCount = arguments[2].toInt(&ok);
        if (!ok || (playersCount != 3 && playersCount != 4))
        {
            error(ERROR_INCORRECT_PLAYERS_COUNT);
            usage();
            return ERROR_INCORRECT_PLAYERS_COUNT;
        }

        uint port = arguments[3].toUInt(&ok);
        if (!ok || port <= 0 || port > 65535)
        {
            error(ERROR_INCORRECT_PORT_VALUE);
            usage();
            return ERROR_INCORRECT_PORT_VALUE;
        }

        Server server;
        server.start(playersCount, port);
        server.waitForStart();
        if (server.isListening())
        {
            int result = app.exec();
            server.stop();
            qDebug() << "Server has been stopped";
            return result;
        }
        else
        {
            QTextStream qerr(stderr);
            qerr << server.errorString() << endl;
            error(ERROR_INCORRECT_CONNECTION);
            usage();
            return ERROR_INCORRECT_CONNECTION;
        }
    }
    else
    {
        error(ERROR_INCORRECT_ARGUMENTS);
        usage();
        return ERROR_INCORRECT_ARGUMENTS;
    }
}
