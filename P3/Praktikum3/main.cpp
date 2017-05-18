#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>


int main(int argc, char *argv[]) {

    QSurfaceFormat format;
    format.setVersion(4, 4); // requested OpenGL version; you might have to change this
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setOption(QSurfaceFormat::DeprecatedFunctions);
    format.setOption(QSurfaceFormat::DebugContext);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

//Lösungen Aufgabe 3.1
/*
 * 1. Zugewiesen bekomme ich OpenGL Version 2.0, meine interne Grafikkarte würde allerdings
 *    bis zu 4.4 unterstützen, aufgrund meiner deprecated Funktionen die ich aufrufe finde
 *    ich dies allerdings nachvollziehbar.
 *
 * 2. Ja, klappt.
 *
 * 3. done.
 *
 * 4. wird gemacht.*
 *
 */
