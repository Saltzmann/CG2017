#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QKeyEvent>

class MyGLWidget : public QOpenGLWidget
{
    Q_OBJECT
private:
    float _angle;
    float _XOffset;
    float _YOffset;
    float _ZOffset;
public:
    MyGLWidget(QWidget *parent);
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent *event);
public slots:
    void receiveRotationZ(int degrees);
};

#endif // MYGLWIDGET_H
