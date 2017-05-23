#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>


int main(int argc, char *argv[]) {

    QSurfaceFormat format;
    format.setVersion(4, 4); // requested OpenGL version; you might have to change this
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    //format.setProfile(QSurfaceFormat::CoreProfile);
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
 * 11.Core Profile = keine Funktionen die in 3.0 als veraltet markiert wurden
 *    (drawElements, VertexAttributePointer usw funktionieren nicht mehr).
 *    Deprecated Functions deaktiviert =  sinngemäß das selbe wie zuvor
 *
 *    Generell deprecated: ShaderInput Manipulationen und einige "settings" mit glEnable usw.
 *    Ich habe einmal jede Funktion mit dem Debugger durchgeskipped aber da wir diese nun nicht
 *    ersetzen müssen habe ich keine genaueren Kommentare gemacht. In aktuellen und eigenen
 *    Applikationen verwendet man natürlich dann die aktuellen Funktionen, alleine schon aus
 *    Sicht der Performance.
 */
