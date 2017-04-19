#include "myglwidget.h"

#define WINDOW_CAPTION "Computergrafik Praktikum"
#define WINDOW_XPOS 50
#define WINDOW_YPOS 50
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 480
#define UPDATE_RATE 60

// global variable for engine purposes
// not pretty and hardware-dependent, but gets the job done
//unsigned int counter = 0;

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent) {

}

int MyGLWidget::heightForWidth(int width) {
    return width;
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

    //glViewport(0,0,width, height);

    // Set viewport to cover the whole window
    //glViewport(0, 0, width, height);

    // Set projection matrix to a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(-0.05, 0.05, -0.05, 0.05, 0.1, 100.0);
}

void MyGLWidget::paintGL() {
    // Clear buffer to set color and alpha
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply model view transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Objet 7 Einheiten in den Raum "weg" schieben
    glTranslatef(0.0f, 0.0f, -7.0f);

    //A1.7
    glRotatef(45, 0.f, 0.f, 1.f);

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

    // Execute all issued GL commands
    //glFlush(); // replace with glutSwapBuffers() for double buffered mode

    // Increment counter
    //counter++;
}
