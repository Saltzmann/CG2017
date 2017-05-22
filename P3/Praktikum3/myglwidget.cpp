#include "myglwidget.h"

#define WINDOW_CAPTION "Computergrafik Praktikum"
#define WINDOW_XPOS 50
#define WINDOW_YPOS 50
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 480
#define UPDATE_RATE 60
#define INITIAL_CAMERA_OFFSET 50.0f
#define TICKRATE 1000/60


MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent),
                                          _vbo(QOpenGLBuffer::VertexBuffer),
                                          _ibo(QOpenGLBuffer::IndexBuffer)
                                        {
    _angle = 0;
    _XOffset = 0.f;
    _YOffset = 0.f;
    _ZOffset = INITIAL_CAMERA_OFFSET;

    setFocusPolicy(Qt::StrongFocus);

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
        //_SetAngle(--_angle);
    }
    else if(event->key() == (int)Qt::Key_E) {
        //_SetAngle(++_angle);
    }
    else if(event->key() == Qt::Key_Space) {
        if(_myTimer->isActive()) {
            _myTimer->stop();
        }
        else {
            _myTimer->start(TICKRATE);
        }
    }
    else {
        QOpenGLWidget::keyPressEvent(event);
    }
    event->accept();
    this->update();
    //Wenn wir die Sliderwerte über die Tastatur verändern passiert nichts mehr...
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

    glEnable(GL_TEXTURE_2D);
    // Schlagen Sie mirrored() nach - warum und wie wird die Textur gespiegelt?
    _qTex = new QOpenGLTexture(QImage(":/earthmap1k.jpg").mirrored());
    // Schlagen Sie die Filter in der Dokumentation nach, wofür ist Mip-Mapping gut?
    _qTex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _qTex->setMagnificationFilter(QOpenGLTexture::Linear);
    // Anm.: Wenn qTex->textureId() == 0 ist, dann ist etwas schief gegangen
    Q_ASSERT(_qTex->textureId() != 0);

    CelestialBody* gay = new CelestialBody(12756.3,
                             23.44,
                             1.f,
                             365.f,
                             20,
                             true,
                             "earthmap1k.jpg");

    //test
    test = new CelestialBody(12756.3,
                             23.44,
                             1.f,
                             365.f,
                             50,
                             true,
                             "earthmap1k.jpg",
                             gay);

    _myTimer = new QTimer(this);
    connect(_myTimer, SIGNAL(timeout()),
            test, SLOT(update()));
    //Timer ist mehr Performance einstellung als visuell
    _myTimer->start(1000/60);

    // Lade die Shader-Sourcen aus externen Dateien (ggf. anpassen)
    _shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/default330.vert");
    _shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/default330.frag");
    // Kompiliere und linke die Shader-Programme
    _shaderProgram.link();

    _initializeCelestialBodies();

    _fillBuffers();
}

void MyGLWidget::resizeGL(int width, int height) {
    // Compute aspect ratio
    height = (height == 0) ? 1 : height;
    //GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set viewport to cover the whole window
    glViewport(0, 0, width, height);
}

void MyGLWidget::_initializeVBOs() {
    // Lade Modell aus Datei
    // Anm.: Linux/Unix kommt mit einem relativen Pfad hier evtl. nicht zurecht
    ModelLoader model;
    bool res = model.loadObjectFromFile("C:/Users/Tobias/Documents/GitHub/CG2017/P3/Praktikum3/sphere_high.obj");
    // Wenn erfolgreich, generiere VBO und Index-Array
    if (res) {
        // Frage zu erwartende Array-Längen ab
        _vboLength = model.lengthOfVBO();
        _iboLength = model.lengthOfIndexArray();
        // Generiere VBO und Index-Array
        _vboData = new GLfloat[_vboLength];
        _indexData = new GLuint[_iboLength];
        model.genVBO(_vboData);
        model.genIndexArray(_indexData);
        qDebug() << "Models laden erfolgreich!";
    }
    else {
        // Modell konnte nicht geladen werden
        qDebug() << "Models laden fehlgeschlagen!";
        Q_ASSERT(false);
    }

    _vbo.allocate(_vboData, sizeof(GLfloat) * _vboLength);
    _ibo.allocate(_indexData, sizeof(GLuint) * _iboLength);
}

void MyGLWidget::_initializeCelestialBodies() {
    //todo
    //planeten und monde erstellen und verknüpfen
    //timer nicht vergessen
}

void MyGLWidget::_fillBuffers() {
    _vbo.create();
    _ibo.create();

    _vbo.bind();
    _ibo.bind();

    _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    _initializeVBOs();

    _vbo.release();
    _ibo.release();
}

void MyGLWidget::paintGL() {

    //stuff

    //skybox?

    //sun.RenderWithChildren(&_shaderProgram,
    //                       _matrixStack,
    //                       _vboData,
    //                       _indexData);

    //vbo, ibo und shader sollten gebunden bleiben können

    // Clear buffer to set color and alpha
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Mache die Buffer im OpenGL-Kontext verfügbar
    _shaderProgram.bind();
    _vbo.bind();
    _ibo.bind();

    // Lokalisiere bzw. definiere die Schnittstelle für die Eckpunkte
    int attrVertices = 0;
    // Lokalisiere bzw. definiere die Schnittstelle für die Textur
    int attrTexCoords = 3;
    // Aktiviere die Verwendung der Attribute-Arrays
    _shaderProgram.enableAttributeArray(attrVertices);
    _shaderProgram.enableAttributeArray(attrTexCoords);
    //_shaderProgram.enableAttributeArray(attrColors);

    // Lokalisiere bzw. definiere die Schnittstelle für die Transformationsmatrix
    // Die Matrix kann direkt übergeben werden, da setUniformValue für diesen Typ
    // überladen ist

    QMatrix4x4 matrix;

    int unifModviewMatrix = 1;
    matrix.setToIdentity();
    matrix = test->getOrbitalTransformationMatrix();
    _matrixStack.push(matrix);

    int unifPersMatrix = 0;
    matrix.setToIdentity();
    matrix.perspective(60.0, 16.0/9.0, 0.1, 10000.0);
    matrix.translate(-_XOffset, -_YOffset, -_ZOffset);
    _matrixStack.push(matrix);

    //Ab hier Stack abarbeiten
    matrix = _matrixStack.top();
    _matrixStack.pop();

    _shaderProgram.setUniformValue(unifPersMatrix, matrix);

    matrix = _matrixStack.top(); // glPopMatrix
    matrix *= test->getRotationTransformationMatrix();
    _matrixStack.pop();

    _shaderProgram.setUniformValue(unifModviewMatrix, matrix);

    //qDebug() << "MatrixStack is now empty: " << _matrixStack.empty();
    Q_ASSERT(_matrixStack.empty());

    // Binde die Textur an den OpenGL-Kontext
    _qTex->bind();

    // Übergebe die Textur an die Uniform-Variable
    // Die 0 steht dabei für die verwendete Unit (0=Standard)
    _shaderProgram.setUniformValue("texture", 0);

    // Fülle die Attribute-Buffer mit den korrekten Daten
    int offset = 0;
    size_t stride = 12 * sizeof(GLfloat);
    _shaderProgram.setAttributeBuffer(attrVertices, GL_FLOAT, offset, 4, stride);
    offset += 8 * sizeof(GLfloat);
    _shaderProgram.setAttributeBuffer(attrTexCoords, GL_FLOAT, offset, 4, stride);

    glDrawElements(GL_TRIANGLES, _iboLength, GL_UNSIGNED_INT, 0);

    // Deaktiviere die Verwendung der Attribute-Arrays
    _shaderProgram.disableAttributeArray(attrVertices);
    _shaderProgram.disableAttributeArray(attrTexCoords);
    //_shaderProgram.disableAttributeArray(attrColors);

    // Löse die Textur aus dem OpenGL-Kontext
    _qTex->release();

    _vbo.release();
    _ibo.release();
    // Löse das Shader-Programm
    _shaderProgram.release();


    this->update();
}

void MyGLWidget::onMessageLogged(QOpenGLDebugMessage message) {
    if(message.type() == QOpenGLDebugMessage::PerformanceType &&
       message.severity() == QOpenGLDebugMessage::LowSeverity) {
        //anti spam meassure ... demnächst gerne gefixed
        return;
    }
    qDebug() << message;
}
