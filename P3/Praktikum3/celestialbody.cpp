#include "celestialbody.h"

CelestialBody::CelestialBody(double diameter, float axialTilt,
                             float rotationPeriod, float orbitalPeriod,
                             double orbitalRadius, bool CCWRotation, QString textureFileName,
                             CelestialBody *inOrbitOf) {
    //Simple start values
    this->_diameter = diameter;
    this->_axialTilt = axialTilt;
    this->_rotationPeriod = rotationPeriod;
    this->_orbitalPeriod = orbitalPeriod;
    this->_orbitalRadius = orbitalRadius;
    this->_inOrbitOf = inOrbitOf;
    this->_CCWRotation = CCWRotation;

    //calculated values
    if(_CCWRotation) {
        this->_rotationalAnglePerDay = 360.f / _rotationPeriod * -1;
    }
    else {
        this->_rotationalAnglePerDay = 360.f / _rotationPeriod;
    }

    this->_orbitalAnglePerDay = 360.f / _orbitalPeriod;
    //others
    _setTexture(textureFileName);
    _currentOrbitalAngle = 0.f;
    _currentRotationalAngle = 0.f;
}


double CelestialBody::getDiameter() {
    return _diameter;
}

float CelestialBody::getAxialTilt() {
    return _axialTilt;
}

float CelestialBody::getRotationPeriod() {
    return _rotationPeriod;
}

double CelestialBody::getOrbitalPeriod() {
    return _orbitalPeriod;
}

double CelestialBody::getOrbitalRadius() {
    return _orbitalRadius;
}

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

bool CelestialBody::isInOrbitOf() {
    return(_inOrbitOf != nullptr);
}

float CelestialBody::getRotationalAnglePerDay() {
    return _rotationalAnglePerDay;
}

float CelestialBody::getOrbitalAnglePerDay() {
    return _orbitalAnglePerDay;
}

void CelestialBody::update() {
    _currentRotationalAngle += _rotationalAnglePerDay;
    _currentOrbitalAngle += _orbitalAnglePerDay;
}

QMatrix4x4 CelestialBody::getTransformationMatrix() {
    QMatrix4x4 matrix;

    matrix.setToIdentity();



    //set own rotation axis
    matrix.rotate(_axialTilt, 0.f, 0.f, 1.f);

}

QOpenGLTexture* CelestialBody::getTexture() {
    return _qTex;
}
