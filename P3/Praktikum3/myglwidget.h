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
#include <QMatrix4x4>
#include <modelloader.h>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
private:
    int _angle;
    float _XOffset;
    float _YOffset;
    float _ZOffset;
    void _SetAngle(int degrees);
    QTimer* _myTimer;

    //Praktikum 3
    QOpenGLBuffer _vbo;
    QOpenGLBuffer _ibo;
    //Arrays für Buffer
    GLfloat *_vertices;
    GLubyte *_indices;
    //Shader
    QOpenGLShaderProgram _shaderProgram;
    //MatrixStack, damit überall erreichbar und nicht immmer "neu"
    std::stack<QMatrix4x4> _matrixStack;

    GLfloat* _vboData;
    GLuint* _indexData;
    // GLuint statt GLubyte, da viele Modelle groß sind
    unsigned int _vboLength;
    unsigned int _iboLength;

    //The Debug Logger Ladies and Gents
    QOpenGLDebugLogger* debugLogger;
public:
    MyGLWidget(QWidget *parent);
    void initializeGL();
    void buildGeometry();
    void fillBuffers();
    void paintGL();
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void initializeVBOs();
public slots:
    void receiveRotationZ(int degrees);
    void autoRotateZ();
    void onMessageLogged(QOpenGLDebugMessage message);
signals:
    void sendZOffset(int value);
};

#endif // MYGLWIDGET_H
