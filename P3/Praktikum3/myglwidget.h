#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QWheelEvent>
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
    float _XOffset;
    float _YOffset;
    float _ZOffset;
    float _viewingAngleX;
    float _viewingAngleY;

    QTimer* _myTimer;

    //Planet Stuff
    CelestialBody* sun;

    //Praktikum 3
    QOpenGLBuffer _vbo;
    QOpenGLBuffer _ibo;

    //Shader
    QOpenGLShaderProgram _shaderProgram;
    //MatrixStack, damit überall erreichbar und nicht immmer "neu"
    std::stack<QMatrix4x4> _matrixStack;

    //Modelle
    GLfloat* _vboData;
    GLuint* _indexData; // GLuint statt GLubyte, da viele Modelle groß sind
    unsigned int _vboLength;
    unsigned int _iboLength;

    //The Debug Logger Ladies and Gents
    QOpenGLDebugLogger* debugLogger;
    void _initializeVBOs();
    void _initializeCelestialBodies();
    void _fillBuffers();
public:
    MyGLWidget(QWidget *parent);
    void initializeGL();
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);    
public slots:
    void onMessageLogged(QOpenGLDebugMessage message);
signals:
    void sendSpeedFactor(int value);
};

#endif // MYGLWIDGET_H
