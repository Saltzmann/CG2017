#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

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
    // Handle für VBO (Eckpunkte + Farben)
    GLuint _vboHandle;
    // Handle für BO der Indizes für die zu zeichnenden Elemente
    GLuint _indicesHandle;
public:
    MyGLWidget(QWidget *parent);
    void initializeGL();
    void buildGeometry();
    void fillBuffers();
    void paintGL();
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
public slots:
    void receiveRotationZ(int degrees);
    void autoRotateZ();
signals:
    void sendZOffset(int value);
};

#endif // MYGLWIDGET_H
