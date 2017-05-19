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
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //schwarz

    // Lade die Shader-Sourcen aus externen Dateien (ggf. anpassen)
    _shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/default330.vert");
    _shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/default330.frag");
    // Kompiliere und linke die Shader-Programme
    _shaderProgram.link();

    buildGeometry();

    fillBuffers();
}

void MyGLWidget::resizeGL(int width, int height) {
    // Compute aspect ratio
    height = (height == 0) ? 1 : height;
    //GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set viewport to cover the whole window
    glViewport(0, 0, width, height);
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

    // Mache die Buffer im OpenGL-Kontext verfügbar
    _shaderProgram.bind();
    _vbo.bind();
    _ibo.bind();

    // Lokalisiere bzw. definiere die Schnittstelle für die Eckpunkte
    int attrVertices = 0;
    // Lokalisiere bzw. definiere die Schnittstelle für die Farben
    int attrColors = 1;
    // Aktiviere die Verwendung der Attribute-Arrays
    _shaderProgram.enableAttributeArray(attrVertices);
    _shaderProgram.enableAttributeArray(attrColors);
    // Lokalisiere bzw. definiere die Schnittstelle für die Transformationsmatrix
    // Die Matrix kann direkt übergeben werden, da setUniformValue für diesen Typ
    // überladen ist

    QMatrix4x4 matrix;

    int unifModviewMatrix = 1;
    matrix.setToIdentity();
    matrix.rotate(45.f + (float)_angle, 0.0, 0.0, 1.0);
    _matrixStack.push(matrix);

    int unifPersMatrix = 0;
    matrix.setToIdentity();
    matrix.perspective(60.0, 16.0/9.0, 0.1, 100.0);
    matrix.translate(0.0, 0.0, - 7.0);
    _matrixStack.push(matrix);

    //Ab hier Stack abarbeiten
    matrix = _matrixStack.top();
    _matrixStack.pop();

    _shaderProgram.setUniformValue(unifPersMatrix, matrix);

    matrix = _matrixStack.top(); // glPopMatrix
    _matrixStack.pop();

    _shaderProgram.setUniformValue(unifModviewMatrix, matrix);

    qDebug() << "MatrixStack is now empty: " << _matrixStack.empty();
    Q_ASSERT(_matrixStack.empty());

    // Fülle die Attribute-Buffer mit den korrekten Daten
    int offset = 0;
    int stride = 8 * sizeof(GLfloat);
    _shaderProgram.setAttributeBuffer(attrVertices, GL_FLOAT, offset, 4, stride);
    offset += 4 * sizeof(GLfloat);
    _shaderProgram.setAttributeBuffer(attrColors, GL_FLOAT, offset, 4, stride);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0); //wieso hier 0?

    // Deaktiviere die Verwendung der Attribute-Arrays
    _shaderProgram.disableAttributeArray(attrVertices);
    _shaderProgram.disableAttributeArray(attrColors);

    _vbo.release();
    _ibo.release();
    // Löse das Shader-Programm
    _shaderProgram.release();


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
    qDebug() << message;
}
