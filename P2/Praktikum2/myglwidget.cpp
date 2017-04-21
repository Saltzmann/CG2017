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

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    _angle = 0.f;
    _XOffset = 0.f;
    _YOffset = 0.f;
    _ZOffset = INITIAL_CAMERA_OFFSET;
    //
    setFocusPolicy(Qt::StrongFocus);
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
    else {
        QOpenGLWidget::keyPressEvent(event);
    }
    this->update();
    //Wenn wir die Sliderwerte über die Tastatur verändern passiert nichts mehr...
}

void MyGLWidget::receiveRotationZ(int degrees) {
    //qDebug() << qintptr(degrees);
    _angle = degrees;
}

void MyGLWidget::initializeGL() {
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(-0.05*aspect, 0.05*aspect, -0.05, 0.05, 0.1, 100.0);
}

void MyGLWidget::paintGL() {
    // Clear buffer to set color and alpha
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply model view transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Objet 7 Einheiten in den Raum "weg" schieben
    glTranslatef(-_XOffset, -_YOffset, -7.0f);

    //Rotieren?
    glRotated(_angle, 0.f, 0.f, 1.f);

    // Set color for drawing
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    // Draw shape
    glBegin(GL_TRIANGLES); //x,y,z -> z = Ebene
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f); //rot
        glVertex3f( 1.0f, -1.0f,  0.0f); //unten rechts
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); //weiß
        glVertex3f( 1.0f,  1.0f,  0.0f); //oben rechts
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f); //blau
        glVertex3f(-1.0f,  1.0f,  0.0f); //oben links

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f); //rot
        glVertex3f( 1.0f, -1.0f,  0.0f); //unten rechts
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f); //blau
        glVertex3f(-1.0f,  1.0f,  0.0f); //oben links
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f); //schwarz
        glVertex3f(-1.0f, -1.0f,  0.0f); //unten links
    glEnd();

    this->update();
    //this->repaint();
}
