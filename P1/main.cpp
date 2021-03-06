/* Computergrafik Praktikum
 * Klassisches OpenGL mit freeGLUT
 * FH Aachen, 2015
 */

#include <GL/glut.h>

#define WINDOW_CAPTION "Computergrafik Praktikum"
#define WINDOW_XPOS 50
#define WINDOW_YPOS 50
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 480
#define UPDATE_RATE 60


// global variable for engine purposes
// not pretty and hardware-dependent, but gets the job done
unsigned int counter = 0;


// OpenGL settings initialization
void init()
{
    glEnable(GL_DEPTH_TEST);
    //A1.6
    glEnable(GL_CULL_FACE); //Option 1
    glDepthFunc(GL_LEQUAL);

    //A1.9
    //glShadeModel(GL_FLAT);
    glShadeModel(GL_SMOOTH);


    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearDepth(1.0f);

    //A1.3
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //weiß
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //schwarz
}


// Handler for timer event
void update(int v)
{
    // Schedule a draw event
    glutPostRedisplay();

    // Needs to be registered again
    glutTimerFunc((unsigned int) 1000.0f / UPDATE_RATE, update, v);

    //A1.11
    //glutTimerFunc ist nur bedingt gut für eine flüssige Drehung, da TimerFunc immer
    //registiert werden müssen und dann lower bound nach der Zeit ausgeführt werden
    //man hat also nicht immer eine flüssige Bewegung
    //und es könnte zusätzlich noch hardwareabhängig sein, wann diese Funktion auf unterschiedlichen
    //Rechnern ausgeführt wird
    //besser könnte sein das Redraw immer in festen Abständen zu machen
    //zum Beispiel in dem es durch so etwas wie den (Glut)MainLoop gesteuert wird
}


// Handler for window resize and creation event
void reshape(GLsizei width, GLsizei height)
{
   // Compute aspect ratio
   height = (height == 0) ? 1 : height;
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   // Set viewport to cover the whole window
   glViewport(0, 0, width, height);

   // Set projection matrix to a perspective projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   //A1.10
   //gluPerspective(45.0f, aspect, 0.1, 100.0);
   glOrtho(-3.0f, 3.0f, -3.0f, 3.0f, -5, 100);
   //Der 2D Körper hat keinen Fluchtpunkt, daher kann man den Unterschied zwischen
   //orthogonaler und perpektivischer Darstellung nicht sehen.
}


// Handler for window draw event
void draw()
{
    // Clear buffer to set color and alpha
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply model view transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Objet 7 Einheiten in den Raum "weg" schieben
    glTranslatef(0.0f, 0.0f, -7.0f);

    //A1.5
    //Rotation entlang der Z Achse um 45°
    //glRotatef(45, 0, 0, 1);

    //A1.7
    glRotatef(45 + counter*0.5f, 0.f, 0.f, 1.f);

    //A1.6
    //glRotatef(180, 0.f, 1.f, 0.f);

    //A1.6
    //glCullFace(GL_FRONT); //Option 2


    // Set color for drawing
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    //A1.4
    //Koordinatenursprung liegt auf der Mitte der Hypothenuse

    //A1.8 und A1.9
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
    glFlush(); // replace with glutSwapBuffers() for double buffered mode

    // Increment counter
    counter++;
}


// main function
int main(int argc, char **argv)
{
    // initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE); // GLUT_DOUBLE for double buffered mode

    // Create output window
    glutInitWindowPosition(WINDOW_XPOS, WINDOW_YPOS);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow(WINDOW_CAPTION);

    // Register callback handlers for GLUT events
    glutReshapeFunc(reshape);
    glutDisplayFunc(draw); // Note: not called continuously
    glutTimerFunc((unsigned int) 1000.0f / UPDATE_RATE, update, 0); // Note: inaccurate, but good enough here

    // Initialize OpenGL parameters
    init();

    // Jump into the main drawing loop
    glutMainLoop();

    return 0;
}
