#include "celestialbody.h"

CelestialBody::CelestialBody(QString planetName,
                             double diameter, float axialTilt,
                             float rotationPeriod, float orbitalPeriod,
                             double orbitalRadius, QString textureFileName) {
    //Simple start values
    this->_name = planetName;
    this->_diameter = diameter;
    this->_axialTilt = axialTilt;
    this->_rotationPeriod = rotationPeriod;
    this->_orbitalPeriod = orbitalPeriod;
    this->_orbitalRadius = orbitalRadius;

    //calculated values
    //Periods sind immer in Tagen angegeben = Zeit für 360 Grad Drehung
    //rotation pro Tag = 360 Grad / Periode
    //Mal 365,25 = Rotation in Grad pro Jahr
    // das mal Beschleunigungs/Simulationsfaktor = ergebnis

    this->_rotationalAnglePerTick = ((360.0 / _rotationPeriod) * 365.25) * SIMYEARS_PER_TICK;

    if(_orbitalPeriod > 0.0) {
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
void CelestialBody::addOrbitingCelestialBody(CelestialBody *child) {
    Orbiting.push_back(child);
}

bool CelestialBody::hasCelestialBodiesOrbiting() {
    return (!Orbiting.empty());
}

void CelestialBody::RenderWithChildren(QMatrix4x4 ctm,
                                       const QMatrix4x4 &viewMatrix,
                                       const QMatrix4x4 &projectionMatrix,
                                       QOpenGLShaderProgram& shader,
                                       unsigned int const &iboLength) {
    //skybox muss 90° umd z gedreht werden
    //perspective ist einzelne Matrix siehe Shader

    //orbTrans auf Stack
    int unifProjMatrix = 0;
    int unifViewMatrix = 1;
    int unifModelMatrix = 2;

    //Ctm ist call bei value daher sollte dies gehen, dass sie hier verändert wird
    _getOrbitalTransformationMatrix(ctm);

    //gucken ob kinder -> ausführen mit top of Stack
    if(!Orbiting.empty()) {
        for(CelestialBody* x : Orbiting) { //gucken ob er foreach kann!!
            x->RenderWithChildren(ctm,
                                  viewMatrix,
                                  projectionMatrix,
                                  shader,
                                  iboLength);
        }
    }

    _getRotationTransformationMatrix(ctm);

    shader.setUniformValue(unifProjMatrix, projectionMatrix);
    shader.setUniformValue(unifViewMatrix, viewMatrix);
    shader.setUniformValue(unifModelMatrix, ctm);

    //dann Textur binden
    _qTex->bind();

    //an shader übergeben
    shader.setUniformValue("texture", 0);

    if(_name == "Skybox") { //frontface wechseln
        glFrontFace(GL_CW);
    }

    //zeichnen lassen
    glDrawElements(GL_TRIANGLES, iboLength, GL_UNSIGNED_INT, 0);

    if(_name == "Skybox") { //frontface zurück wechseln
        glFrontFace(GL_CCW);
    }

    // Löse die Textur aus dem OpenGL-Kontext
    _qTex->release();
}

//PRIVATE METHODS
void CelestialBody::_setTexture(QString filename) {
    _qTex = new QOpenGLTexture(QImage(":/" + filename).mirrored());
    _qTex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _qTex->setMagnificationFilter(QOpenGLTexture::Linear);
    // Anm.: Wenn qTex->textureId() == 0 ist, dann ist etwas schief gegangen
    Q_ASSERT(_qTex->textureId() != 0);
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

void CelestialBody::_getOrbitalTransformationMatrix(QMatrix4x4 &matrix) {

    //andersherum...sprich lies von unten nach oben

    //betrifft ORBIT
    //um Sonne/Planet rotieren
    matrix.rotate(_currentOrbitalAngle, 0.f, 1.f, 0.f);

    //in den Orbit translatieren
    matrix.translate(_orbitalRadius * SCALE_FACTOR, 0.f, 0.f);
}

void CelestialBody::_getRotationTransformationMatrix(QMatrix4x4 &matrix) {

    //betrifft EIGENROTATION
    //orbitale rotation dreht auch Planet, da dies nicht gewollt ist vorher gegenrotieren
    matrix.rotate(-_currentOrbitalAngle, 0.f, 1.f, 0.f);

    //wieder um achse neigen
    matrix.rotate(_axialTilt, 0.f, 0.f, 1.f);

    //um rotationsachse rotieren
    matrix.rotate(-_currentRotationalAngle, 0.f, 1.f, 0.f);

    //textur ist bereits geneigt also zurückneigen
    matrix.rotate(-_axialTilt, 0.f, 0.f, 1.f);

    matrix.scale((_diameter / 2.0) * SCALE_FACTOR); //radius skalieren
    //qDebug() << "Skalierter Radius von" << _name << "beträgt" << ((_diameter / 2.0) * SCALE_FACTOR);
    //qDebug() << "Berechneter Abstand von" << _name << "zur Sonne =" << (_orbitalRadius * SCALE_FACTOR);
}

