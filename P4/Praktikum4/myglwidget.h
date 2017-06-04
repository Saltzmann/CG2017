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
    QVector3D _viewOffset;
    QVector3D _viewDirection;
    QVector3D const _upVector;
    QVector3D _rightVector;
    QVector2D _oldMousePosition;

    QTimer* _myTimer;

    //Planet Stuff
    CelestialBody* _galaxy;
    CelestialBody* sun;

    //Model Stuff
    QOpenGLBuffer _vbo;
    QOpenGLBuffer _ibo;
    bool _hasTextureCoords;
    GLfloat* _vboData;
    GLuint* _indexData; // GLuint statt GLubyte, da viele Modelle groß sind
    unsigned int _vboLength;
    unsigned int _iboLength;
    unsigned char _VertOffset;
    unsigned char _normOffset;
    unsigned char _TexCoordOffset;
    size_t _stride;

    //Shader
    QOpenGLShaderProgram _shaderProgram;

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
