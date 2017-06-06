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
#include <sun.h>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
private:
    //Kameraführungs-Variablen
    float _speedFactor;
    QVector3D _viewOffset;
    QVector3D _viewDirection;
    QVector3D const _upVector;
    QVector3D _rightVector;
    QVector2D _oldMousePosition;

    //QTimer
    QTimer* _myTimer; //SimTimer
    QTimer* _secondTimer; //FPSTimer

    //Root Element zum Himmelskörper rendern
    CelestialBody* _galaxy;

    //Model und Buffer-Object bezogene Daten
    QOpenGLBuffer _vbo;
    QOpenGLBuffer _ibo;
    bool _hasTextureCoords;
    GLfloat* _vboData;
    GLuint* _indexData; // GLuint statt GLubyte, da viele Modelle groß sind
    unsigned int _vboLength;
    unsigned int _iboLength;
    unsigned short _vertOffset;
    unsigned short _normOffset;
    unsigned short _texCoordOffset;
    size_t _stride;

    //Shader
    QOpenGLShaderProgram* _defaultShaderProgram;
    QOpenGLShaderProgram* _heatShimmerShaderProgram;
    QOpenGLShaderProgram* _phongShaderProgram;

    //GUI Hilfsvariablen
    unsigned int _fpsCounter;
    float _actualFPS;
    float _linMod;
    float _expMod;

    //Debug Logger für erweiterte Fehlermeldungen
    QOpenGLDebugLogger* debugLogger;

    //(ausgelagerte) Hilfsfunktionen - hauptsächlich zur Übersichtlichkeit
    void _initializeVBOs();
    void _initializeCelestialBodies();
    void _fillBuffers();
public:
    //Konstruktor
    MyGLWidget(QWidget *parent);
    //QT OpenGL Overrides
    void initializeGL();
    void paintGL();
    //Mouse und Keyboard-Event Overrides
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
public slots:
    void onMessageLogged(QOpenGLDebugMessage message);
    void resetFPSCounter();
    void recieveLinModUpdate(double value);
    void recieveExpModUpdate(double value);
signals:
    void sendSpeedFactor(int value);
    void sendFPSValue(int value);
};

#endif // MYGLWIDGET_H
