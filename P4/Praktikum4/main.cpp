#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>


int main(int argc, char *argv[]) {

    QSurfaceFormat format;
    format.setVersion(4, 4); //meine OpenGL Version
    format.setProfile(QSurfaceFormat::CompatibilityProfile); //sowohl dies
    format.setOption(QSurfaceFormat::DeprecatedFunctions); //als auch dies
    // muss leider wegen ein paar Aufrufen (hauptsächlich in den Shadern) drin bleiben...
    format.setOption(QSurfaceFormat::DebugContext);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
