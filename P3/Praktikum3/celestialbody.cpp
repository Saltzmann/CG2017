#include "celestialbody.h"

CelestialBody::CelestialBody(QString planetName,
                             double diameter, float axialTilt,
                             float rotationPeriod, float orbitalPeriod,
                             double orbitalRadius, bool CCWRotation, QString textureFileName) {
    //Simple start values
    this->_name = planetName;
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

void CelestialBody::RenderWithChildren(int attrVertices,
                                       int attrTexCoords,
                                       QOpenGLShaderProgram *shader,
                                       std::stack<QMatrix4x4> *matrixStack,
                                       unsigned int iboLength,
                                       QVector3D viewingOffsets,
                                       float viewingAngle) {
    QMatrix4x4 matrix;

    //orbTrans auf Stack
    int unifModviewMatrix = 1;
    matrix.setToIdentity();
    matrix = _getOrbitalTransformationMatrix();
    matrixStack->push(matrix);

    //gucken ob kinder -> ausführen mit top of Stack
    if(hasCelestialBodiesOrbiting()) {
        for(CelestialBody* x : Orbiting) { //gucken ob er foreach kann!!
            x->RenderWithChildren(attrVertices,
                                  attrTexCoords,
                                  shader,
                                  matrixStack,
                                  iboLength,
                                  viewingOffsets,
                                  viewingAngle);
        }
    }

    //rotTrans mit top of Stack multiplizieren
    matrix.setToIdentity();
    matrix = _getRotationTransformationMatrix();
    matrixStack->top() *= matrix;

    //dann persMat auf Stack
    int unifPersMatrix = 0;
    matrix.setToIdentity();
    matrix.perspective(60.0, 16.0/9.0, 0.1, 10000.0);
    matrix.translate(viewingOffsets);
    matrixStack->push(matrix);

    //dann stack abbauen
    //erst persmat top, pop, zu shader
    matrix = matrixStack->top();
    matrixStack->pop();
    shader->setUniformValue(unifPersMatrix, matrix);

    //dann modview, top, pop, zu shader
    matrix = matrixStack->top();
    matrixStack->pop();
    shader->setUniformValue(unifModviewMatrix, matrix);

    //dann Textur binden
    _qTex->bind();

    //an shader übergeben
    shader->setUniformValue("texture", 0);

    //vertices und texcoords an shader geben
    int offset = 0;
    size_t stride = 12 * sizeof(GLfloat);
    shader->setAttributeBuffer(attrVertices, GL_FLOAT, offset, 4, stride);
    offset += 8 * sizeof(GLfloat);
    shader->setAttributeBuffer(attrTexCoords, GL_FLOAT, offset, 4, stride);

    //zeichnen lassen
    glDrawElements(GL_TRIANGLES, iboLength, GL_UNSIGNED_INT, 0);

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

QMatrix4x4 CelestialBody::_getOrbitalTransformationMatrix() {
    QMatrix4x4 matrix1;

    //andersherum...sprich lies von unten nach oben

    matrix1.setToIdentity();

    //betrifft ORBIT
    //um Sonne/Planet rotieren
    matrix1.rotate(_currentOrbitalAngle, 0.f, 1.f, 0.f);

    //in den Orbit translatieren
    matrix1.translate(_orbitalRadius * SCALE_FACTOR, 0.f, 0.f);

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

    matrix2.scale((_diameter / 2.0) * SCALE_FACTOR); //radius skalieren
    qDebug() << "Skalierter Radius von" << _name << "beträgt" << ((_diameter / 2.0) * SCALE_FACTOR);
    qDebug() << "Berechneter Abstand von" << _name << "zur Sonne =" << (_orbitalRadius * SCALE_FACTOR);

    return matrix2;
}

