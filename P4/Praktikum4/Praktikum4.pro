#-------------------------------------------------
#
# Project created by QtCreator 2017-05-03T16:04:20
#
#-------------------------------------------------

QT       += core gui opengl

# Link against OpenGL for Qt >= 5.5
LIBS += -lopengl32
unix: LIBS += -lassimp

# Windows can't deal with assimp as a system library for some reason
win32: LIBS += -LC:/Users/Tobias/Documents/GitHub/CG2017/assimp-win-mingw32/code/ -lassimp
win32: INCLUDEPATH += C:/Users/Tobias/Documents/GitHub/CG2017/assimp-win-mingw32/include
win32: DEPENDPATH += C:/Users/Tobias/Documents/GitHub/CG2017/assimp-win-mingw32/include



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Praktikum4
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    myglwidget.cpp \
    modelloader.cpp \
    celestialbody.cpp

HEADERS  += mainwindow.h \
    myglwidget.h \
    modelloader.h \
    celestialbody.h

FORMS    += mainwindow.ui

RESOURCES += \
    shader.qrc \
    textures.qrc

DISTFILES += \
    norm130.fsh \
    norm330.vsh \
    heatshimmer330.frag
