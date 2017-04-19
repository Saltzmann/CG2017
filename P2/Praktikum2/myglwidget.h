#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>

class MyGLWidget : public QOpenGLWidget
{
    Q_OBJECT
private:
    float _angle;
    void repaint();
public:
    MyGLWidget(QWidget *parent);
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void rotate();
};

#endif // MYGLWIDGET_H
