#include <QApplication>
#include "qt/window/qt_window_system.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    QtWindowSystem window;
    window.show();

    return app.exec();
}