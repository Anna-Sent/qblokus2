#include "app.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    qRegisterMetaTypeStreamOperators<ClientInfo>("ClientInfo");
    qRegisterMetaTypeStreamOperators<QList<ClientInfo> >("QList<ClientInfo>");

    QApplication app(argc, argv);
    App *dialog = new App;
    dialog->show();
    return app.exec();
}
