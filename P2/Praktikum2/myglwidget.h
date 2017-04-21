#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QWheelEvent>

class MyGLWidget : public QOpenGLWidget
{
    Q_OBJECT
private:
    int _angle;
    float _XOffset;
    float _YOffset;
    float _ZOffset;
    void _SetAngle(int degrees);
public:
    MyGLWidget(QWidget *parent);
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
public slots:
    void receiveRotationZ(int degrees);
signals:
    void sendZOffset(int value);
};

#endif // MYGLWIDGET_H
