#include "myglwidget.h"

#define UPDATE_RATE 60
#define INITIAL_CAMERA_OFFSET 500.0f
#define INITIAL_SPEED_FACTOR 5
#define TICKRATE 1000/UPDATE_RATE
#define CAMERA_TURN_SPEED 1.f/5.f


MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent),
                                          _upVector(0.f, 1.f, 0.f),
                                          _vbo(QOpenGLBuffer::VertexBuffer),
                                          _ibo(QOpenGLBuffer::IndexBuffer)
                                        {
    //Kamera initialisieren
    _speedFactor = INITIAL_SPEED_FACTOR;
    _viewOffset = QVector3D(0.f, 0.f, INITIAL_CAMERA_OFFSET);
    _viewDirection = QVector3D(0.f, 0.f, -1.f);
    _rightVector = QVector3D(1.f, 0.f, 0.f);

    //Shader initialisieren
    _defaultShaderProgram = new QOpenGLShaderProgram();
    _heatShimmerShaderProgram = new QOpenGLShaderProgram();
    _phongShaderProgram = new QOpenGLShaderProgram();

    //Keyboard und Mouse Einstellungen
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    //Debug Output Versionsnummer etc.
    QSurfaceFormat fmt = this->format();
    qDebug().nospace() << "OpenGL " << fmt.majorVersion() << "." << fmt.minorVersion();
    qDebug().noquote() << "Profile:" << QMetaEnum::fromType<QSurfaceFormat::OpenGLContextProfile>().valueToKey(fmt.profile());
    qDebug().noquote() << "Options:" << QMetaEnum::fromType<QSurfaceFormat::FormatOption>().valueToKeys(fmt.options());
    qDebug().noquote() << "Renderable Type:" << QMetaEnum::fromType<QSurfaceFormat::RenderableType>().valueToKey(fmt.renderableType());
    qDebug().noquote() << "Swap Behavior:" << QMetaEnum::fromType<QSurfaceFormat::SwapBehavior>().valueToKey(fmt.swapBehavior());
    qDebug() << "Swap Interval:" << fmt.swapInterval();

    //GUI Anzeige/Einstell-Werte initialisieren
    _fpsCounter = 0;
    _linMod = 0.40f;
    _expMod = 2.10f;
}

void MyGLWidget::wheelEvent(QWheelEvent *event) {
    int numDegrees = (event->angleDelta() / 8).y() * -1; // durch 8 weil dann Angabe in Grad (* -1) für speed + nach oben

    if (numDegrees != 0) {
        int numSteps = numDegrees / 15; //Jeder Step = 15°
        _speedFactor += (float)numSteps * 0.1; //0.1 als Modifikator damit man nicht zu schnell zoomt
        if(_speedFactor < 0) { //Werte unter 0 würden die Steuerung invertieren daher reset auf 0
            _speedFactor = 0.f;
        }
        emit sendSpeedFactor(_speedFactor);
    }
    event->accept();
    this->update();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event) {
    QVector2D currentMousePosition = QVector2D(event->windowPos());
    QVector2D mouseDelta =  currentMousePosition - _oldMousePosition;
    _oldMousePosition = currentMousePosition;

    _rightVector = QVector3D::crossProduct(_viewDirection, _upVector);
    QMatrix4x4 rotationMatrix;
    rotationMatrix.setToIdentity();
    rotationMatrix.rotate(-mouseDelta.x() * CAMERA_TURN_SPEED, _upVector);
    rotationMatrix.rotate(-mouseDelta.y() * CAMERA_TURN_SPEED, _rightVector);

    _viewDirection = QVector3D(rotationMatrix * _viewDirection.toVector4D()).normalized();
    _rightVector = QVector3D::crossProduct(_viewDirection, _upVector).normalized();

    event->accept();
    this->update();
}

void MyGLWidget::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case (int)Qt::Key_W: //nach vorne
         _viewOffset += _viewDirection *_speedFactor;
        break;
    case (int)Qt::Key_S: //nach hinten
        _viewOffset -= _viewDirection *_speedFactor;
        break;
    case (int)Qt::Key_D: //nach rechts (strafe)
        _viewOffset += _rightVector *_speedFactor;
        break;
    case (int)Qt::Key_A: //nach links (strafe)
        _viewOffset -= _rightVector *_speedFactor;
        break;
    case (int)Qt::Key_R: //nach oben (strafe)
        _viewOffset += _upVector *_speedFactor;
        break;
    case (int)Qt::Key_F: //nach unten (strafe)
        _viewOffset -= _upVector *_speedFactor;
        break;
    case (int)Qt::Key_B: //Kamera kommplett zurücksetzen
        _viewOffset = QVector3D(0.f, 0.f, INITIAL_CAMERA_OFFSET);
        _viewDirection = QVector3D(0.f, 0.f, -1.f);
        _speedFactor = INITIAL_SPEED_FACTOR;
        break;
    case (int)Qt::Key_N: //Nur Blickrichtung zurücksetzen
        _viewDirection = QVector3D(0.f, 0.f, -1.f);
        break;
    case (int)Qt::Key_H: //Planeten Rotation und Orbit stoppen/starten
        if(_myTimer->isActive()) {
            _myTimer->stop();
        }
        else {
            _myTimer->start(TICKRATE);
        }
        break;
    default:
        e->ignore();
        return;
    }
    e->accept();
    this->update();
}

void MyGLWidget::initializeGL() {
    //gewisse OpenGL Funktionen aktiveren (Funktion von QT
    initializeOpenGLFunctions();

    //DebugLogger initialisieren (könnte vermutlich auch in den Konstruktor)
    debugLogger = new QOpenGLDebugLogger(this);

    connect(debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
            SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger->initialize()) {
        debugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
        debugLogger->enableMessages();
    }

    //OpenGL Flags setzen / Funktionen aktivieren
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearDepth(1.0f);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //weiß, damit man den Unterschied zur Skybox sieht

    //Timer allokieren
    _myTimer = new QTimer(this);
    _secondTimer = new QTimer(this);

    //fpsCounter connecten
    connect(_secondTimer, SIGNAL(timeout()),
            this, SLOT(resetFPSCounter()));

    //Jetzt shader zu shader-Programmen hinzufügen

    // default shader
    _defaultShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/default330.vert");
    _defaultShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/default330.frag");

    //heatshimmer shader
    _heatShimmerShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/default330.vert");
    _heatShimmerShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/heatshimmer330.frag");

    //Phong shader
    _phongShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/phong330.vert");
    _phongShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/phong330.frag");

    //Kompiliere und linke die Shader-Programme
    _defaultShaderProgram->link();
    _heatShimmerShaderProgram->link();
    _phongShaderProgram->link();

    //Ausgelagerte Initialierung der Himmelskörper mit jeweiligen Kenndaten + Textur und genutztem Shader
    _initializeCelestialBodies();

    //oben gesetzte Timer starten
    _myTimer->start(TICKRATE);
    _secondTimer->start(1000);

    //VBO und IBO initialisern und Mesh-Daten laden
    _fillBuffers();
}

void MyGLWidget::_initializeVBOs() {
    // Lade Modell aus Datei
    ModelLoader model;
    //Ich bekomme es hier auf Teufel komm raus nicht hin, dass er einen relativen Pfad nimmt...
    bool res = model.loadObjectFromFile("C:/Users/Tobias/Documents/GitHub/CG2017/P4/Praktikum4/sphere_high.obj");
    _hasTextureCoords = model.hasTextureCoordinates();
    Q_ASSERT(model.hasTextureCoordinates()); //aktuell MUSS das Model Texturkoordinaten haben (auch wenn später abgefangen)

    if (res) { //Wenn erfolgreich, generiere VBO und Index-Array
        // Frage zu erwartende Array-Längen ab
        _vboLength = model.lengthOfVBO();
        _iboLength = model.lengthOfIndexArray();
        // Initialisiere VBO und Index-Array
        _vboData = new GLfloat[_vboLength];
        _indexData = new GLuint[_iboLength];
        //Generiere VBO und IBO Data mit vertices, normals, texCoords
        model.genVBO(_vboData);
        model.genIndexArray(_indexData);
        qDebug() << "Models laden erfolgreich!";
    }
    else {
        // Modell konnte nicht geladen werden
        qDebug() << "Models laden fehlgeschlagen!";
        Q_ASSERT(false); //gewollter Programmabbruch
    }

    //Versatz und Schrittweiten-Anpassung mit und ohne Texturkoordinaten
    if(_hasTextureCoords) {
        _vertOffset = 0;
        _normOffset = _vertOffset + 4 * sizeof(GLfloat);
        _texCoordOffset = _vertOffset + 8 * sizeof(GLfloat);
        _stride = 12 * sizeof(GLfloat);
    }
    else {
        _vertOffset = 0;
        _normOffset = _vertOffset + 4 * sizeof(GLfloat);
        _texCoordOffset = -1;
        _stride = 8 * sizeof(GLfloat);
    }

    //Schreibe Daten in Buffer-Objekte
    _vbo.allocate(_vboData, sizeof(GLfloat) * _vboLength);
    _ibo.allocate(_indexData, sizeof(GLuint) * _iboLength);
}

void MyGLWidget::_initializeCelestialBodies() {
    //Setze Rotations-, Orbit-, Textur- und Shader-Informationen
    //für alle zu rendernden Himmelskörper
    //(nebenbei) baue Baumstruktur auf für Renderreihenfolge

    CelestialBody* sun = nullptr;
    CelestialBody* planet = nullptr;
    CelestialBody* moon = nullptr;

    _galaxy = new CelestialBody("Skybox",
                                SKYBOX_DIAMETER,
                                0,
                                -250000,
                                0,
                                0,
                                "milkyway5.jpg",
                                _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            _galaxy, SLOT(update()));

    sun = new Sun("Sun",
                   1392684,
                   7.25,
                   -25.38,
                   0,
                   0,
                   "sun1k.jpg",
                   _heatShimmerShaderProgram,
                   "sun2k.jpg");

    connect(_myTimer, SIGNAL(timeout()),
            sun, SLOT(update()));

    _galaxy->addOrbitingCelestialBody(sun);

    planet = new CelestialBody("Mercury",
                               4879.4,
                               0.01,
                               -59,
                               88,
                               800000,
                               "mercurymap.jpg",
                               _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Venus",
                               12103.6,
                               177.36,
                               243,
                               225,
                               850000,
                               "venusmap.jpg",
                               _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Earth",
                               12756.32,
                               23.44,
                               -1,
                               365.25,
                               900000,
                               "earthmap1k.jpg",
                               _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    moon = new CelestialBody("Moon",
                             3476,
                             6.68,
                             -27,
                             27,
                             12000,
                             "moonmap1k.jpg",
                             _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            moon, SLOT(update()));

    planet->addOrbitingCelestialBody(moon);

    planet = new CelestialBody("Mars",
                               6792.4,
                               25.19,
                               -1,
                               687,
                               950000,
                               "mars_1k_color.jpg",
                               _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    moon = new CelestialBody("Phobos",
                              1000, //faktor 50 zu viel aber sonst sieht man nix
                              0,
                              -0.32,
                              0.32,
                              5000,
                              "phobosbump.jpg",
                              _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            moon, SLOT(update()));

    planet->addOrbitingCelestialBody(moon);

    moon = new CelestialBody("Deimos",
                              600, //faktor 50 zu viel aber sonst sieht man nix
                              0,
                              -1.26,
                              1.26,
                              6000,
                              "deimosbump.jpg",
                              _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            moon, SLOT(update()));

    planet->addOrbitingCelestialBody(moon);

    planet = new CelestialBody("Jupiter",
                               142984,
                               3.13,
                               -0.42,
                               4333,
                               1200000,
                               "jupiter2_1k.jpg",
                               _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Saturn",
                               120536,
                               26.73,
                               -0.42,
                               10752,
                               1500000,
                               "saturnmap.jpg",
                               _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Uranus",
                               51118,
                               97.77,
                               0.72,
                               30664,
                               1750000,
                               "uranusmap.jpg",
                               _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);

    planet = new CelestialBody("Neptune",
                               49528,
                               28.32,
                               -0.67,
                               60142,
                               1900000,
                               "neptunemap.jpg",
                               _phongShaderProgram);

    connect(_myTimer, SIGNAL(timeout()),
            planet, SLOT(update()));

    sun->addOrbitingCelestialBody(planet);
}

void MyGLWidget::_fillBuffers() {
    //VBO und IBO zum ersten mal erstellen und für die Datenspeicherung
    //an den aktuellen Kontext binden

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

    //Clear buffer to set color and alpha
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Mache die Buffer im OpenGL-Kontext verfügbar
    //Dies geschieht hier nur 1x da alle Himmelskörper das gleiche
    //Model verwenden und es so nicht jedes mal neu eingebunden werden muss
    _vbo.bind();
    _ibo.bind();

    //Benötigte Matrizen bereitstellen
    QMatrix4x4 projectionMatrix, viewMatrix, modelMatrix;

    //Modelmatrix zur Einheitsmatrix machen - wird später modifiziert
    modelMatrix.setToIdentity();

    //ViewMatrix setzen, bleibt während aller aufrufe der Kinder konstant
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(_viewOffset, _viewOffset + _viewDirection, _upVector);

    //ProjectionMatrix setzen, bleibt während aller aufrufe der Kinder konstant
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(60.0, 16.0/9.0, 0.1, 10000.0);

    //Wurzelelement startet Aufruf zum rendern
    _galaxy->RenderWithChildren(modelMatrix,
                                viewMatrix,
                                projectionMatrix,
                                _iboLength,
                                _vertOffset,
                                _normOffset,
                                _texCoordOffset,
                                _stride,
                                _hasTextureCoords,
                                _linMod,
                                _expMod);

    //VBO und IBO bis zum nächsten Rendervorgang wieder freigeben
    _vbo.release();
    _ibo.release();

    //1 Durchlauf = 1 Frameupdate -> FPS zählen zum berechnen
    _fpsCounter++;
    emit sendFPSValue(_actualFPS);
    this->update();
}

void MyGLWidget::onMessageLogged(QOpenGLDebugMessage message) {
    if(message.type() == QOpenGLDebugMessage::PerformanceType &&
       message.severity() == QOpenGLDebugMessage::LowSeverity) {
       //anti spam meassure ... demnächst gerne gefixed
       //Fehler der Auftriff: "API_ID_REDUNDANT_FBO performance warning has been generated. Redundant state change in glBindFramebuffer API call, FBO 1"
       return;
    }
    qDebug() << message;
}

void MyGLWidget::resetFPSCounter() {
    //FPS Counter jede Sekunde zurücksetzen / Anzeigewert anpassen
    _actualFPS = _fpsCounter;
    _fpsCounter = 0;
}

void MyGLWidget::recieveLinModUpdate(double value) {
    //Slot um LinMod Wert von GUI zu bekommen und zu ändern
    _linMod = value;
}

void MyGLWidget::recieveExpModUpdate(double value) {
    //Slot um ExpMod Wert von GUI zu bekommen und zu ändern
    _expMod = value;
}
