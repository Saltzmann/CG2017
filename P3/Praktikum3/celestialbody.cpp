#include "celestialbody.h"

CelestialBody::CelestialBody(double diameter, float axialTilt,
                             float rotationPeriod, float orbitalPeriod,
                             double orbitalRadius, bool CCWRotation, QString textureFileName) {
    //Simple start values
    this->_diameter = diameter;
    this->_axialTilt = axialTilt;
    this->_rotationPeriod = rotationPeriod;
    this->_orbitalPeriod = orbitalPeriod;
    this->_orbitalRadius = orbitalRadius;
    this->_CCWRotation = CCWRotation;

    //calculated values
    //Periods sind immer in Tagen angegeben = Zeit für 360 Grad Drehung
    //rotation pro Tag = 360 Grad / Periode
    //Mal 365,25 = Rotation in Grad pro Jahr
    // das mal Beschleunigungs/Simulationsfaktor = ergebnis

    this->_rotationalAnglePerTick = ((360.0 / _rotationPeriod) * 365.25) * SIMYEARS_PER_TICK;

    if(_CCWRotation) {
        _rotationalAnglePerTick *= -1;
    }

    if(_inOrbitOf) {
        this->_orbitalAnglePerTick = ((360.f / _orbitalPeriod) * 365.25) * SIMYEARS_PER_TICK;
    }
    else {
        this->_orbitalAnglePerTick = 0.0;
    }

    qDebug() << "_rotationalAnglePerTick : " << _rotationalAnglePerTick;
    qDebug() << "_orbitalAnglePerTick : " << _orbitalAnglePerTick;

    //others
    _setTexture(textureFileName);
    _currentOrbitalAngle = 0.f;
    _currentRotationalAngle = 0.f;
}

//PUBLIC METHODS
void CelestialBody::addOrbitingCelestialBody(CelestialBody child) {
    Orbiting.push_back(child);
}

bool CelestialBody::hasCelestialBodiesOrbiting() {
    return (!Orbiting.empty());
}

void CelestialBody::RenderWithChildren(QOpenGLShaderProgram *shader,
                                       std::stack<QMatrix4x4> *matrixStack,
                                       GLfloat *vboData,
                                       GLuint *indexData) {
    //TODO aus paintGL() hier hin ausgelagert, außerdem Aufruf der Funktion bei den Kindern
}

//PRIVATE METHODS
void CelestialBody::_setTexture(QString filename) {
    _qTex = new QOpenGLTexture(QImage(":/" + filename).mirrored());
    _qTex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _qTex->setMagnificationFilter(QOpenGLTexture::Linear);
    // Anm.: Wenn qTex->textureId() == 0 ist, dann ist etwas schief gegangen
    Q_ASSERT(_qTex->textureId() != 0);
}

double CelestialBody::_getScale() {
    return (_diameter / 4879.4); //size of mercury the smallest planet
}

void CelestialBody::update() {
    //qDebug() << "update called";
    //currentRotationalAngle
    _currentRotationalAngle += _rotationalAnglePerTick;
    if(_currentRotationalAngle >= 360) {
        _currentRotationalAngle -= 360.f;
    }
    else if(_currentRotationalAngle <= 0) {
        _currentRotationalAngle += 360;
    }
    //currentOrbitalAngle
    _currentOrbitalAngle += _orbitalAnglePerTick;
    if(_currentOrbitalAngle >= 360) {
        _currentOrbitalAngle -= 360.f;
    }
    else if(_currentOrbitalAngle <= 0) {
        _currentOrbitalAngle += 360;
    }
    //qDebug() << "currentRotAngle : " << _currentRotationalAngle;
    //qDebug() << "currentOrbAngle : " << _currentOrbitalAngle;
}

QMatrix4x4 CelestialBody::_getOrbitalTransformationMatrix() {
    QMatrix4x4 matrix1;

    //andersherum...sprich lies von unten nach oben

    matrix1.setToIdentity();

    //betrifft ORBIT
    //um Sonne/Planet rotieren
    matrix1.rotate(_currentOrbitalAngle, 0.f, 1.f, 0.f);

    //in den Orbit translatieren
    matrix1.translate(_orbitalRadius, 0.f, 0.f);

    return matrix1;
}

QMatrix4x4 CelestialBody::_getRotationTransformationMatrix() {
    QMatrix4x4 matrix2;
    matrix2.setToIdentity();

    //betrifft EIGENROTATION
    //orbitale rotation dreht auch Planet, da dies nicht gewollt ist vorher gegenrotieren
    matrix2.rotate(-_currentOrbitalAngle, 0.f, 1.f, 0.f);

    //wieder um achse neigen
    matrix2.rotate(_axialTilt, 0.f, 0.f, 1.f);

    //um rotationsachse rotieren
    matrix2.rotate(_currentRotationalAngle, 0.f, 1.f, 0.f);

    //textur ist bereits geneigt also zurückneigen
    matrix2.rotate(-_axialTilt, 0.f, 0.f, 1.f);

    matrix2.scale(_getScale());
    //qDebug() << "Wird skaliert um Faktor : " << _getScale();

    return matrix2;
}

