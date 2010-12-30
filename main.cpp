#include "app.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    App *dialog = new App;
    return app.exec();
}
