#include "celestialbody.h"

CelestialBody::CelestialBody(QString planetName,
                             double diameter,
                             float axialTilt,
                             float rotationPeriod,
                             float orbitalPeriod,
                             double orbitalRadius,
                             QString mainTextureFileName,
                             QOpenGLShaderProgram *myShader,
                             QString secondaryTextureFileName) {
    //Simple start values
    this->_name = planetName;
    this->_diameter = diameter;
    this->_axialTilt = axialTilt;
    this->_rotationPeriod = rotationPeriod;
    this->_orbitalPeriod = orbitalPeriod;
    this->_orbitalRadius = orbitalRadius;
    this->_shader = myShader;

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

    //qDebug() << "_rotationalAnglePerTick : " << _rotationalAnglePerTick;
    //qDebug() << "_orbitalAnglePerTick : " << _orbitalAnglePerTick;

    //others
    _mainTexture = nullptr;
    _secondaryTexture = nullptr;

    _setMainTexture(mainTextureFileName);
    if(secondaryTextureFileName != "") {
        _setSecondaryTexture(secondaryTextureFileName);
    }
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
                                       unsigned int const &iboLength,
                                       unsigned short const &vertOffset,
                                       unsigned short const &normOffset,
                                       unsigned short const &texCoordOffset,
                                       size_t const &stride,
                                       bool const &hasTextureCoords) {
    //qDebug() << "RenderWithChildren: CelBod";
    //Ctm ist call bei value daher sollte dies gehen, dass sie hier verändert wird
    _getOrbitalTransformationMatrix(ctm);

    //gucken ob kinder -> ausführen mit top of Stack
    if(!Orbiting.empty()) {
        for(CelestialBody* x : Orbiting) { //gucken ob er foreach kann!!
            x->RenderWithChildren(ctm,
                                  viewMatrix,
                                  projectionMatrix,
                                  iboLength,
                                  vertOffset,
                                  normOffset,
                                  texCoordOffset,
                                  stride,
                                  hasTextureCoords);
        }
    }

    _getRotationTransformationMatrix(ctm);

    //Matrix Locations
    int unifProjMatrix = 0;
    int unifViewMatrix = 1;
    int unifModelMatrix = 2;

    //Matrix ops fertig nun shader
    _shader->bind();

    // Lokalisiere bzw. definiere die Schnittstelle für die Eckpunkte
    int attrVertices = 0;
    // Lokalisiere bzw. definiere die Schnittstelle für die Normalen
    int attrNorms = 1;
    // Lokalisiere bzw. definiere die Schnittstelle für die Textur
    int attrTexCoords = 2;

    // Aktiviere die Verwendung der Attribute-Arrays
    _shader->enableAttributeArray(attrVertices);
    _shader->enableAttributeArray(attrNorms);
    if(hasTextureCoords) {
        _shader->enableAttributeArray(attrTexCoords);
    }

    // Fülle die Attribute-Buffer mit den korrekten Daten
    _shader->setAttributeBuffer(attrVertices, GL_FLOAT, vertOffset, 4, stride);
    _shader->setAttributeBuffer(attrNorms, GL_FLOAT, normOffset, 4, stride);
    if(hasTextureCoords) {
        _shader->setAttributeBuffer(attrTexCoords, GL_FLOAT, texCoordOffset, 4, stride);
    }

    _shader->setUniformValue(unifProjMatrix, projectionMatrix);
    _shader->setUniformValue(unifViewMatrix, viewMatrix);
    _shader->setUniformValue(unifModelMatrix, ctm);

    //dann Textur binden
    _mainTexture->bind(0);
    if(_secondaryTexture != nullptr) {
        _secondaryTexture->bind(1);
    }

    //an shader übergeben
    _shader->setUniformValue("diffuseMap", 0);
    if(_secondaryTexture != nullptr) {
        _shader->setUniformValue("distortionMap", 1);
    }

    if(_name == "Skybox") { //frontface wechseln
        glFrontFace(GL_CW);
    }

    //zeichnen lassen
    glDrawElements(GL_TRIANGLES, iboLength, GL_UNSIGNED_INT, 0);

    if(_name == "Skybox") { //frontface zurück wechseln
        glFrontFace(GL_CCW);
    }

    // Deaktiviere die Verwendung der Attribute-Arrays
    _shader->disableAttributeArray(attrVertices);
    _shader->disableAttributeArray(attrNorms);
    if(hasTextureCoords) {
        _shader->disableAttributeArray(attrTexCoords);
    }

    //Shader lösen
    _shader->release();

    // Löse die Textur aus dem OpenGL-Kontext
    _mainTexture->release();
    if(_secondaryTexture != nullptr) {
        _secondaryTexture->release();
    }
}

//PRIVATE METHODS
void CelestialBody::_setMainTexture(QString filename) {
    _mainTexture = new QOpenGLTexture(QImage(":/" + filename).mirrored());
    _mainTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _mainTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    // Anm.: Wenn qTex->textureId() == 0 ist, dann ist etwas schief gegangen
    Q_ASSERT(_mainTexture->textureId() != 0);
}

void CelestialBody::_setSecondaryTexture(QString filename) {
    _secondaryTexture = new QOpenGLTexture(QImage(":/" + filename).mirrored());
    _secondaryTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _secondaryTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    _secondaryTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
    // Anm.: Wenn qTex->textureId() == 0 ist, dann ist etwas schief gegangen
    Q_ASSERT(_secondaryTexture->textureId() != 0);
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

