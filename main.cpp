#include "app.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    App *dialog = new App;
    dialog->show();
    return app.exec();
}
