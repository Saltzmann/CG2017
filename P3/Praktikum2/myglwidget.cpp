#include "myglwidget.h"
#include <qdebug.h>

#define WINDOW_CAPTION "Computergrafik Praktikum"
#define WINDOW_XPOS 50
#define WINDOW_YPOS 50
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 480
#define UPDATE_RATE 60
#define INITIAL_CAMERA_OFFSET 7.0f

// global variable for engine purposes
// not pretty and hardware-dependent, but gets the job done
//unsigned int counter = 0;

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent),
                                          _vbo(QOpenGLBuffer::VertexBuffer),
                                          _ibo(QOpenGLBuffer::IndexBuffer)
                                        {
    _angle = 0;
    _XOffset = 0.f;
    _YOffset = 0.f;
    _ZOffset = INITIAL_CAMERA_OFFSET;
    //
    setFocusPolicy(Qt::StrongFocus);

    _myTimer = new QTimer(this);
    connect(_myTimer, SIGNAL(timeout()),
            this, SLOT(autoRotateZ()));
    _myTimer->start(1000/60);
    qDebug() << _myTimer->isActive();
}

void MyGLWidget::wheelEvent(QWheelEvent *event) {
    int numDegrees = (event->angleDelta() / 8).y(); // durch 8 weil dann Angabe in Grad

    if (numDegrees != 0) {
        int numSteps = numDegrees / 15;
        _ZOffset += (float)numSteps * 0.01f;
        emit sendZOffset(_ZOffset);
        //qDebug() << _ZOffset;
    }
    event->accept();
    this->update();
}

void MyGLWidget::keyPressEvent(QKeyEvent *event) {
    //Oben
    if (event->key() == (int)Qt::Key_W || event->key() == (int)Qt::Key_Up) {
        _YOffset += 0.1f;
    }
    //Unten
    else if(event->key() == (int)Qt::Key_S || event->key() == (int)Qt::Key_Down) {
        _YOffset -= 0.1f;
    }
    //Links
    else if(event->key() == (int)Qt::Key_A || event->key() == (int)Qt::Key_Left) {
        _XOffset  -= 0.1f;
    }
    //Rechts
    else if(event->key() == (int)Qt::Key_D || event->key() == (int)Qt::Key_Right) {
        _XOffset += 0.1f;
    }
    else if(event->key() == (int)Qt::Key_Q) {
        _SetAngle(--_angle);
    }
    else if(event->key() == (int)Qt::Key_E) {
        _SetAngle(++_angle);
    }
    else if(event->key() == Qt::Key_Space) {
        if(_myTimer->isActive()) {
            _myTimer->stop();
        }
        else {
            _myTimer->start(1000/60);
        }
    }
    else {
        QOpenGLWidget::keyPressEvent(event);
    }
    event->accept();
    this->update();
    //Wenn wir die Sliderwerte über die Tastatur verändern passiert nichts mehr...
}

void MyGLWidget::receiveRotationZ(int degrees) {
    //qDebug() << qintptr(degrees);
    _SetAngle(degrees);
}

void MyGLWidget::initializeGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_FLAT); //DEPRECATED
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //schwarz
}

void MyGLWidget::resizeGL(int width, int height) {
    // Compute aspect ratio
    //height = (height == 0) ? 1 : height;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set viewport to cover the whole window
    glViewport(0, 0, width, height);

    // Set projection matrix to a perspective projection
    glMatrixMode(GL_PROJECTION); //DEPRECATED
    glLoadIdentity();  //DEPRECATED
    glFrustum(-0.05*aspect, 0.05*aspect, -0.05, 0.05, 0.1, 100.0);  //DEPRECATED
}

void MyGLWidget::paintGL() {
    // Clear buffer to set color and alpha
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply model view transformations
    glMatrixMode(GL_MODELVIEW);  //DEPRECATED
    glLoadIdentity();  //DEPRECATED
    //Objekt 7 Einheiten in den Raum "weg" schieben
    glTranslatef(-_XOffset, -_YOffset, -_ZOffset);  //DEPRECATED

    //Rotieren?
    glRotatef(45.f + (float)_angle, 0.f, 1.f, 0.f); //DEPRECATED

    glRotatef(10.f, 1.f, 0.f, 0.f);
    glRotatef(30.f + (float)_angle, 0.f, 0.f, 1.f); //DEPRECATED

    // Set color for drawing
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);  //DEPRECATED

    // Draw shape
    glBegin(GL_QUADS);  //DEPRECATED
        //vorne - rot
        glColor4f(1.f, 0.f, 0.f, 1.f);  //DEPRECATED
            glVertex3i(-1, -1, 1);  //DEPRECATED
            glVertex3i(1, -1, 1); //DEPRECATED
            glVertex3i(1, 1, 1); //DEPRECATED
            glVertex3i(-1, 1, 1); //DEPRECATED

        //links - grün
        glColor4f(0.f, 1.f, 0.f, 1.f); //DEPRECATED
            glVertex3i(-1, 1, 1); //DEPRECATED
            glVertex3i(-1, 1, -1); //DEPRECATED
            glVertex3i(-1, -1, -1); //DEPRECATED
            glVertex3i(-1, -1, 1); //DEPRECATED

        //unten - gelb
        glColor4f(1.f, 1.f, 0.f, 1.f); //DEPRECATED
            glVertex3i(-1, -1, 1); //DEPRECATED
            glVertex3i(-1, -1, -1); //DEPRECATED
            glVertex3i(1, -1, -1); //DEPRECATED
            glVertex3i(1, -1, 1); //DEPRECATED

        //hinten - orange
        glColor4f(1.f, 0.647059f, 0.f, 1.f); //DEPRECATED
            glVertex3i(-1, -1, -1); //DEPRECATED
            glVertex3i(-1, 1, -1); //DEPRECATED
            glVertex3i(1, 1, -1); //DEPRECATED
            glVertex3i(1, -1, -1); //DEPRECATED

        //rechts - blau
        glColor4f(0.f, 0.f, 1.f, 1.f); //DEPRECATED
            glVertex3i(1, -1, -1); //DEPRECATED
            glVertex3i(1, 1, -1); //DEPRECATED
            glVertex3i(1, 1, 1); //DEPRECATED
            glVertex3i(1, -1, 1); //DEPRECATED

        //oben - weiß
        glColor4f(1.f, 1.f, 1.f, 1.f); //DEPRECATED
            glVertex3i(-1, 1, -1); //DEPRECATED
            glVertex3i(-1, 1, 1); //DEPRECATED
            glVertex3i(1, 1, 1); //DEPRECATED
            glVertex3i(1, 1, -1); //DEPRECATED

    glEnd(); //DEPRECATED

    this->update();
    //this->repaint();
}

void MyGLWidget::_SetAngle(int degrees) {
    if(degrees > 359) _angle = 0;
    else if(degrees < 0) _angle = 359;
    else _angle = degrees;
}

void MyGLWidget::autoRotateZ() {
    _angle++;
    this->update();
}
