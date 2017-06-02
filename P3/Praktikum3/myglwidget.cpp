#include "myglwidget.h"

#define UPDATE_RATE 60
#define INITIAL_CAMERA_OFFSET 500.0f
#define INITIAL_SPEED_FACTOR 10
#define TICKRATE 1000/UPDATE_RATE


MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent),
                                          _vbo(QOpenGLBuffer::VertexBuffer),
                                          _ibo(QOpenGLBuffer::IndexBuffer)
                                        {
    _speedFactor = INITIAL_SPEED_FACTOR;
    _viewingOffsets = QVector3D(0.f, 0.f, INITIAL_CAMERA_OFFSET);
    _viewingAngles = QVector3D(0.f, 0.f, 0.f);

    //Fürs Keyboard
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
    int numDegrees = (event->angleDelta() / 8).y() * -1; // durch 8 weil dann Angabe in Grad (* -1) für speed + nach oben

    if (numDegrees != 0) {
        int numSteps = numDegrees / 15;
        _speedFactor += (float)numSteps * 0.1;
        emit sendSpeedFactor(_speedFactor);
        //qDebug() << "Speedfactor:" << _speedFactor;
    }
    event->accept();
    this->update();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event) {
    event->accept();
}

void MyGLWidget::keyPressEvent(QKeyEvent *event) {

    //TODO auch wenn falsch und doof erstmal hier lassen bis Rest gefixed =)

    //nach oben
    if (event->key() == (int)Qt::Key_W) {
        //_YOffset += 1.f * _speedFactor;
        _viewingOffsets.setY(_viewingOffsets.y() + (1.f * _speedFactor));
    }
    //nach unten
    else if(event->key() == (int)Qt::Key_S) {
        //_YOffset -= 1.f * _speedFactor;
        _viewingOffsets.setY(_viewingOffsets.y() - (1.f * _speedFactor));
    }
    //Links (strafe)
    else if(event->key() == (int)Qt::Key_A) {
        //_XOffset  -= 1.f * _speedFactor;
        _viewingOffsets.setX(_viewingOffsets.x() - (1.f * _speedFactor));
    }
    //Rechts (strafe)
    else if(event->key() == (int)Qt::Key_D) {
        //_XOffset += 1.f * _speedFactor;
        _viewingOffsets.setX(_viewingOffsets.x() + (1.f * _speedFactor));
    }
    //nach vorne
    else if (event->key() == (int)Qt::Key_E) {
        //_ZOffset -= 1.f * _speedFactor;
        _viewingOffsets.setZ(_viewingOffsets.z() - (1.f * _speedFactor));
    }
    //nach hinten
    else if(event->key() == (int)Qt::Key_Q) {
        //_ZOffset += 1.f * _speedFactor;
        _viewingOffsets.setZ(_viewingOffsets.z() + (1.f * _speedFactor));
    }
    //Links (turn)
    else if(event->key() == (int)Qt::Key_Left) {
        //_viewingAngleX -= 2.f;
        _viewingAngles.setX(_viewingAngles.x() - 2.f);
    }
    //Rechts (turn)
    else if(event->key() == (int)Qt::Key_Right) {
        //_viewingAngleX += 2.f;
        _viewingAngles.setX(_viewingAngles.x() + 2.f);
    }
    //Hoch (turn)
    else if(event->key() == (int)Qt::Key_Up) {
        //_viewingAngleY -= 2.f;
        _viewingAngles.setY(_viewingAngles.y() - 2.f);
    }
    //Runter (turn)
    else if(event->key() == (int)Qt::Key_Down) {
        //_viewingAngleY += 2.f;
        _viewingAngles.setY(_viewingAngles.y() + 2.f);
    }
    else if(event->key() == (int)Qt::Key_B) { //hard reset
        _viewingOffsets = QVector3D(0.f, 0.f, INITIAL_CAMERA_OFFSET);
        _viewingAngles = QVector3D(0.f, 0.f, 0.f);
        _speedFactor = INITIAL_SPEED_FACTOR;
    }
    else if(event->key() == (int)Qt::Key_N) { //soft reset
        _viewingAngles = QVector3D(0.f, 0.f, 0.f);
        _speedFactor = INITIAL_SPEED_FACTOR;
    }
    else if(event->key() == Qt::Key_H) {
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
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearDepth(1.0f);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //weiß, damit man den Unterschied zur Skybox sieht

    _myTimer = new QTimer(this);    

    // Lade die Shader-Sourcen aus externen Dateien (ggf. anpassen)
    _shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/default330.vert");
    _shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/default330.frag");

    // Kompiliere und linke die Shader-Programme
    _shaderProgram.link();

    _initializeCelestialBodies();

    _myTimer->start(TICKRATE);

    _fillBuffers();
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
    CelestialBody* sun = nullptr;
    CelestialBody* planet = nullptr;
    CelestialBody* moon = nullptr;

    _galaxy = new CelestialBody("Skybox",
                                25000000,
                                90,
                                -2500,
                                0,
                                0,
                                "milkyway.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            _galaxy, SLOT(update()));

    sun = new CelestialBody("Sun",
                            1392684,
                            7.25,
                            -25.38,
                            0,
                            0,
                            "sunmap.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            sun, SLOT(update()));

    _galaxy->addOrbitingCelestialBody(sun);

    planet = new CelestialBody("Mercury",
                               4879.4,
                               0.01,
                               -59,
                               88,
                               800000,
                               "mercurymap.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Venus",
                               12103.6,
                               177.36,
                               243,
                               225,
                               850000,
                               "venusmap.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Earth",
                               12756.32,
                               23.44,
                               -1,
                               365.25,
                               900000,
                               "earthmap1k.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    moon = new CelestialBody("Moon",
                             3476,
                             6.68,
                             -27,
                             27,
                             12000,
                             "moonmap1k.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            moon, SLOT(update()));

    planet->addOrbitingCelestialBody(moon);

    planet = new CelestialBody("Mars",
                               6792.4,
                               25.19,
                               -1,
                               687,
                               950000,
                               "mars_1k_color.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    moon = new CelestialBody("Phobos",
                              1000, //faktor 50 zu viel aber sonst sieht man nix
                              0,
                              -0.32,
                              0.32,
                              5000,
                              "phobosbump.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            moon, SLOT(update()));

    planet->addOrbitingCelestialBody(moon);

    moon = new CelestialBody("Deimos",
                              600, //faktor 50 zu viel aber sonst sieht man nix
                              0,
                              -1.26,
                              1.26,
                              6000,
                              "deimosbump.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            moon, SLOT(update()));

    planet->addOrbitingCelestialBody(moon);

    planet = new CelestialBody("Jupiter",
                               142984,
                               3.13,
                               -0.42,
                               4333,
                               1200000,
                               "jupiter2_1k.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Saturn",
                               120536,
                               26.73,
                               -0.42,
                               10752,
                               1500000,
                               "saturnmap.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Uranus",
                               51118,
                               97.77,
                               0.72,
                               30664,
                               1750000,
                               "uranusmap.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Neptune",
                               49528,
                               28.32,
                               -0.67,
                               60142,
                               1900000,
                               "neptunemap.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);
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

    // Clear buffer to set color and alpha
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Mache die Buffer im OpenGL-Kontext verfügbar
    //sollte gehen da immer die gleichen
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

    //skybox
    glFrontFace(GL_CW);
    glFrontFace(GL_CCW);
    //skybox muss 90° umd z gedreht werden
    //perspective ist einzelne Matrix siehe Shader

    // Fülle die Attribute-Buffer mit den korrekten Daten
    int offset = 0;
    size_t stride = 12 * sizeof(GLfloat);
    _shaderProgram.setAttributeBuffer(attrVertices, GL_FLOAT, offset, 4, stride);
    offset += 8 * sizeof(GLfloat);
    _shaderProgram.setAttributeBuffer(attrTexCoords, GL_FLOAT, offset, 4, stride);

    sun->RenderWithChildren(attrVertices,
                           attrTexCoords,
                           &_shaderProgram,
                           &_matrixStack,
                           _iboLength,
                           QVector3D(-_XOffset, -_YOffset, -_ZOffset),
                           _viewingAngleX,
                           _viewingAngleY);

    // Deaktiviere die Verwendung der Attribute-Arrays
    _shaderProgram.disableAttributeArray(attrVertices);
    _shaderProgram.disableAttributeArray(attrTexCoords);

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
