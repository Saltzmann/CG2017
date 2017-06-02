#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <qdebug.h>
#include <QMetaEnum>
#include <QOpenGLShaderProgram>
#include <stack>
#include <vector>
#include <QMatrix4x4>
#include <modelloader.h>
#include <QOpenGLTexture>
#include <celestialbody.h>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
private:
    float _speedFactor;
    QVector3D _viewingOffsets;
    QVector3D _viewingAngles;

    QTimer* _myTimer;

    //Planet Stuff
    CelestialBody* _galaxy;
    CelestialBody* sun;

    //Praktikum 3
    QOpenGLBuffer _vbo;
    QOpenGLBuffer _ibo;

    //Shader
    QOpenGLShaderProgram _shaderProgram;

    //Modelle
    GLfloat* _vboData;
    GLuint* _indexData; // GLuint statt GLubyte, da viele Modelle groß sind
    unsigned int _vboLength;
    unsigned int _iboLength;

    //The Debug Logger Ladies and Gents
    QOpenGLDebugLogger* debugLogger;

    //Hilfsfunktionen
    void _initializeVBOs();
    void _initializeCelestialBodies();
    void _fillBuffers();
public:
    MyGLWidget(QWidget *parent);
    void initializeGL();
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);    
public slots:
    void onMessageLogged(QOpenGLDebugMessage message);
signals:
    void sendSpeedFactor(int value);
};

#endif // MYGLWIDGET_H
