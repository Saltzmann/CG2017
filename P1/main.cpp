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
    //glEnable(GL_CULL_FACE); //Option 1
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_FLAT);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearDepth(1.0f);
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
   gluPerspective(45.0f, aspect, 0.1, 100.0);
}


// Handler for window draw event
void draw()
{
    // Clear buffer to set color and alpha
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply model view transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -7.0f);

    glPushMatrix(); //you do this to avoid disturbing the transformation matrices for any code following the below lines

    //glTranslatef(0.0f, 0.0f, 7.0f); // translate so that (0, 0, -7) lies at the origin
    glRotatef(180, 0, 1, 0); // now rotate
    //glTranslatef(0.0f, 0.0f, -7.0f); // translate back

    // Set color for drawing
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    //glCullFace(GL_FRONT); //Option 2

    // Draw shape
    glBegin(GL_TRIANGLES); //x,y,z -> z = Ebene
        glVertex3f( 1.0f, -1.0f,  0.0f); //unten rechts
        glVertex3f( 1.0f,  1.0f,  0.0f); //oben rechts
        glVertex3f(-1.0f,  1.0f,  0.0f); //oben links
    glEnd();

    glPopMatrix(); // the old matrix is back

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
