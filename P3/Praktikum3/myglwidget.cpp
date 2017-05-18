#include "myglwidget.h"

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

    setFocusPolicy(Qt::StrongFocus);
    //QTimer Initialisierung
    _myTimer = new QTimer(this);
    connect(_myTimer, SIGNAL(timeout()),
            this, SLOT(autoRotateZ()));
    _myTimer->start(1000/60);
    qDebug() << _myTimer->isActive();

    //Debug Output Versionsnummer etc.
    QSurfaceFormat fmt = this->format();
    qDebug().nospace() << "OpenGL " << fmt.majorVersion() << "." << fmt.minorVersion();
    qDebug().noquote() << "Profile:" << QMetaEnum::fromType<QSurfaceFormat::OpenGLContextProfile>().valueToKey(fmt.profile());
    qDebug().noquote() << "Options:" << QMetaEnum::fromType<QSurfaceFormat::FormatOption>().valueToKeys(fmt.options());
    qDebug().noquote() << "Renderable Type:" << QMetaEnum::fromType<QSurfaceFormat::RenderableType>().valueToKey(fmt.renderableType());
    qDebug().noquote() << "Swap Behavior:" << QMetaEnum::fromType<QSurfaceFormat::SwapBehavior>().valueToKey(fmt.swapBehavior());
    qDebug() << "Swap Interval:" << fmt.swapInterval();
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
    initializeOpenGLFunctions();
    //DebugLogger initialisieren (könnte vermutlich auch in den Konstruktor)
    debugLogger = new QOpenGLDebugLogger(this); // this is a member variable
    connect(debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
            SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger->initialize()) {
        debugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
        debugLogger->enableMessages();
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH); //DEPRECATED
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //schwarz

    buildGeometry();

    fillBuffers();
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

void MyGLWidget::buildGeometry() {

    // Eckpunkte
    // (6 Rechteck mit 4 Eckpunkten mit je 4 Koordinaten und 4 Farbkanälen)
    _vertices = new GLfloat[4*(4+4)];
    // Verwendete Elemente (6 Rechtecke, die durch 2 Dreiecke mit je 3 Vertices dargestellt werden)
    _indices = new GLubyte[2*3]; // für große Meshes lieber GLuint

    // 1. Vertex, Position
    _vertices[0] = 1.0f;
    _vertices[1] = -1.0f;
    _vertices[2] = 0.0f;
    _vertices[3] = 1.0f; // 4. Komponente ist immer 1
    // 1. Vertex, Farbe
    _vertices[4] = 1.0f;
    _vertices[5] = 0.0f;
    _vertices[6] = 0.0f;
    _vertices[7] = 1.0f; // 4. Komponente ist immer 1
    // 2. Vertex, Position
    _vertices[8] = 1.0f;
    _vertices[9] = 1.0f;
    _vertices[10] = 0.0f;
    _vertices[11] = 1.0f; // 4. Komponente ist immer 1
    // 2. Vertex, Farbe
    _vertices[12] = 0.0f;
    _vertices[13] = 1.0f;
    _vertices[14] = 0.0f;
    _vertices[15] = 1.0f; // 4. Komponente ist immer 1
    // 3. Vertex, Position
    _vertices[16] = -1.0f;
    _vertices[17] = 1.0f;
    _vertices[18] = 0.0f;
    _vertices[19] = 1.0f; // 4. Komponente ist immer 1
    // 3. Vertex, Farbe
    _vertices[20] = 1.0f;
    _vertices[21] = 1.0f;
    _vertices[22] = 1.0f;
    _vertices[23] = 1.0f; // 4. Komponente ist immer 1
    // 4. Vertex, Position
    _vertices[24] = -1.0f;
    _vertices[25] = -1.0f;
    _vertices[26] = 0.0f;
    _vertices[27] = 1.0f; // 4. Komponente ist immer 1
    // 4. Vertex, Farbe
    _vertices[28] = 0.0f;
    _vertices[29] = 0.0f;
    _vertices[30] = 1.0f;
    _vertices[31] = 1.0f; // 4. Komponente ist immer 1

    // Initialisiere Elemente (für GL_TRIANGLES)
    _indices[0] = 0;
    _indices[1] = 1;
    _indices[2] = 2;

    _indices[3] = 2;
    _indices[4] = 3;
    _indices[5] = 0;
}

void MyGLWidget::fillBuffers() {
    // Erzeuge VBO, die Parameter verteilen sich hier auf mehrere Methoden
    _vbo.create();
    _vbo.bind();
    _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _vbo.allocate(_vertices, sizeof(GLfloat) * 4 * 8);
    _vbo.release();
    // Erzeuge Index-Buffer
    _ibo.create();
    _ibo.bind();
    _ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _ibo.allocate(_indices, sizeof(GLubyte) * 6);
    _ibo.release();
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
    glRotatef(45.f + (float)_angle, 0.f, 0.f, 1.f); //DEPRECATED

    // *** Rendern ***
    // Mache die Buffer im OpenGL-Kontext verfügbar
    _vbo.bind();
    _ibo.bind();

    // Aktiviere die ClientStates zur Verwendung des Vertex- und Color-Arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    // Setze den Vertex-Pointer (veraltet)
    // Der erste Vertex liegt an Stelle 0 des VBO, hat 4 Komponenten,
    // ist vom Typ GL_FLOAT, und 8*GL_Float Byte liegen zwischen jedem
    // nachfolgenden Eckpunkt
    glVertexPointer(4, GL_FLOAT, sizeof(GLfloat) * 8, (char*) NULL+0);
    // Setze den Color-Pointer (veraltet)
    // Die erste Farbe findet sich beim 17. Byte im VBO, für den Rest s. oben
    glColorPointer(4, GL_FLOAT, sizeof(GLfloat) * 8, (char*) NULL+sizeof(GLfloat)*4);
    // Zeichne die 6 Elemente (Indizes) als Dreiecke
    // Die anderen Parameter verhalten sich wie oben
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0); //wieso hier 0?
    //glDrawArrays(GL_TRIANGLES, 0, 6); // Alternative zu glDrawElements
    // Deaktiviere die Client-States wieder
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // Löse die Buffer aus dem OpenGL-Kontext
    _vbo.release();
    _ibo.release();

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

void MyGLWidget::onMessageLogged(QOpenGLDebugMessage message) {
    //std :: cout << message.message().toStdString() << std::endl;  // use this if qDebug output is not accessible
    qDebug() << message;
}
