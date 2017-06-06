#include "celestialbody.h"

CelestialBody::CelestialBody(QString planetName,
                             double diameter,
                             float axialTilt,
                             float rotationPeriod,
                             float orbitalPeriod,
                             double orbitalRadius,
                             QString mainTextureFileName,
                             QOpenGLShaderProgram *myShader) {
    //Initiale Parameter übertragen
    this->_name = planetName;
    this->_diameter = diameter;
    this->_axialTilt = axialTilt;
    this->_rotationPeriod = rotationPeriod;
    this->_orbitalPeriod = orbitalPeriod;
    this->_orbitalRadius = orbitalRadius;
    this->_shader = myShader;

    //Berechnete Werte:
    //Periods sind immer in Tagen angegeben = Zeit für 360 Grad Drehung
    //rotation pro Tag = 360 Grad / Periode
    //Mal 365,25 = Rotation in Grad pro Jahr
    //dies mal Beschleunigungs/Simulationsfaktor = Rotations/Orbit-Gradzahl pro Tick/Update

    this->_rotationalAnglePerTick = ((360.0 / _rotationPeriod) * 365.25) * SIMYEARS_PER_TICK;

    if(_orbitalPeriod > 0.0) {
        this->_orbitalAnglePerTick = ((360.f / _orbitalPeriod) * 365.25) * SIMYEARS_PER_TICK;
    }
    else {
        this->_orbitalAnglePerTick = 0.0;
    }

    //Textur setzen
    _mainTexture = nullptr;
    _setMainTexture(mainTextureFileName);

    //Start Orbit und Rotation festlegen
    _currentOrbitalAngle = 0.f;
    _currentRotationalAngle = 0.f;
}

void CelestialBody::addOrbitingCelestialBody(CelestialBody *child) {
    //Kindknoten hinzufügen
    Orbiting.push_back(child);
}

void CelestialBody::RenderWithChildren(QMatrix4x4 ctm,
                                       const QMatrix4x4 &viewMatrix,
                                       const QMatrix4x4 &projectionMatrix,
                                       unsigned int const &iboLength,
                                       unsigned short const &vertOffset,
                                       unsigned short const &normOffset,
                                       unsigned short const &texCoordOffset,
                                       size_t const &stride,
                                       bool const &hasTextureCoords,
                                       float const &linMod,
                                       float const &expMod) {
    //ctm ist call bei value daher kann dies hier verändert werden
    //ctm beinhaltet nach dem Aufruf die Positionstransformation, die auch die Kinder betrifft
    _getOrbitalTransformationMatrix(ctm);

    //Prüfen ob der Himmelskörper vom ihm abhängige "Kinder" hat
    if(!Orbiting.empty()) {
        for(CelestialBody* x : Orbiting) { //juhu, c++ kann foreach
            x->RenderWithChildren(ctm,
                                  viewMatrix,
                                  projectionMatrix,
                                  iboLength,
                                  vertOffset,
                                  normOffset,
                                  texCoordOffset,
                                  stride,
                                  hasTextureCoords,
                                  linMod,
                                  expMod);
        }
    }

    //Nach diesem Aufruf beinhaltet ctm auch die Eigenrotation
    _getRotationTransformationMatrix(ctm);

    //Matrix Locations für den Shader
    int unifProjMatrix = 0;
    int unifViewMatrix = 1;
    int unifModelMatrix = 2;
    int unifNormMatrix = 3;

    //Matrix Berechnungen fertig nun shader konfigurieren
    _shader->bind();

    // Lokalisiere bzw. definiere die Schnittstelle für die Eckpunkte, Normalen und Textur
    int attrVertices = 0;
    int attrNorms = 1;
    int attrTexCoords = 2;

    // Aktiviere die Verwendung der Attribute-Arrays
    _shader->enableAttributeArray(attrVertices);
    _shader->enableAttributeArray(attrNorms);
    if(hasTextureCoords) {
        _shader->enableAttributeArray(attrTexCoords);
    }

    // Fülle die Attribute-Buffer mit den korrekten Daten
    _shader->setAttributeBuffer(attrVertices, GL_FLOAT, vertOffset, 4, stride); //VertexPositionen
    _shader->setAttributeBuffer(attrNorms, GL_FLOAT, normOffset, 4, stride); //VertexNormalen
    if(hasTextureCoords) {
        _shader->setAttributeBuffer(attrTexCoords, GL_FLOAT, texCoordOffset, 4, stride); //TexturCoordinaten
    }

    //Uniforms an den Shader übergeben
    _shader->setUniformValue(unifProjMatrix, projectionMatrix); //projektionsMatrix (const)
    _shader->setUniformValue(unifViewMatrix, viewMatrix); //viewMatrix ("const")
    _shader->setUniformValue(unifModelMatrix, ctm); //modelMatrix (immer abhängig vom gerade zu rendernden Himmelskörper)

    _shader->setUniformValue(unifNormMatrix,  (viewMatrix * ctm).normalMatrix()); //berechnete Normalenmatrix
    _shader->setUniformValue(4, (viewMatrix * QVector4D(0.f, 0.f, 0.f, 1.f))); //Position der Lichtquelle
    _shader->setUniformValue(5, QVector3D(0.75f, 0.75f, 0.4f)); //Grundfarbe des Lichts
    _shader->setUniformValue(6, QVector3D(0.7f, 0.7f, 0.7f)); //Farbe des Objekts - fällt durch Textur weg
    _shader->setUniformValue(7, QVector3D(0.05f, 0.05f, 0.05f)); //Umgebungsfarbe/-helligkeit
    _shader->setUniformValue(8, QVector3D(1.f, 1.f, 0.75f)); //Farbe des Highlights
    _shader->setUniformValue(9, 3.f); //Exponent für Highlight-Berechnung
    _shader->setUniformValue(10, 0.f); //Schalter-Variable um Highlights aus zu schalten
    if(_name == "Skybox") {
        _shader->setUniformValue(11, -1.f); //Hack um Normalen für Skybox-Beleuchtung umzudrehen
    }
    else {
        _shader->setUniformValue(11, 1.f); //Normalfall - keine Änderung der Normalen
    }
    _shader->setUniformValue(12, float(SKYBOX_DIAMETER/2.f * SCALE_FACTOR)); //Übergabe des Skybox-Durchmessers als Referenzwert zur Berechnung der Beleuchtungsstärke

    /* ----------------------------------------------------------
    Alte hardgecodete Werte für die Beleuchtungsanpassung
    - hier als Referenz zum nachgucken

    Werte für Galaxiegröße 25 000 000
    _shader->setUniformValue(13, 0.25f);
    _shader->setUniformValue(14, 1.5f);

    Werte für Galaxiegröß  10 000 000
    _shader->setUniformValue(13, 0.35f); //oder 0.40f
    _shader->setUniformValue(14, 3.0f); //oder 2.10f

    //*/
    _shader->setUniformValue(13, linMod); //lineare Beleuchtungsanpassung (abhängig von der Distanz zur Lichtquelle)
    _shader->setUniformValue(14, expMod); //exponentielle Beleuchtungsanpassung (abhängig von der Distanz zur Lichtquelle)

    //Haupt-Textur binden und an shader übergeben
    _mainTexture->bind(0);
    _shader->setUniformValue("diffuseMap", 0);

    if(_name == "Skybox") { //frontface wechseln, wenn Skybox gerendert wird
        glFrontFace(GL_CW);
    }

    //Himmelskörper zeichnen lassen
    glDrawElements(GL_TRIANGLES, iboLength, GL_UNSIGNED_INT, 0);

    if(_name == "Skybox") { //frontface zurück wechseln, wenn rendern der Skybox fertig
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
}

void CelestialBody::_setMainTexture(QString filename) {
    //(ausgelagerte) Hilfsfunktion zum Setzen der Haupt-Textur
    _mainTexture = new QOpenGLTexture(QImage(":/" + filename).mirrored());
    _mainTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _mainTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    Q_ASSERT(_mainTexture->textureId() != 0); //Würde Fehler bedeuten
}

void CelestialBody::update() {
    //currentRotationalAngle anpassen
    _currentRotationalAngle += _rotationalAnglePerTick;
    if(_currentRotationalAngle >= 360) {
        _currentRotationalAngle -= 360.f;
    }
    else if(_currentRotationalAngle <= 0) {
        _currentRotationalAngle += 360;
    }
    //currentOrbitalAngle anpassen
    _currentOrbitalAngle += _orbitalAnglePerTick;
    if(_currentOrbitalAngle >= 360) {
        _currentOrbitalAngle -= 360.f;
    }
    else if(_currentOrbitalAngle <= 0) {
        _currentOrbitalAngle += 360;
    }
}

void CelestialBody::_getOrbitalTransformationMatrix(QMatrix4x4 &matrix) {
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
    //textur ist bereits geneigt also zurückneigen (ist zumindest bei der Erde so, beim Jupiter nicht...daher eiert der...)
    matrix.rotate(-_axialTilt, 0.f, 0.f, 1.f);
    //Radius des Himmelskörpers skalieren
    matrix.scale((_diameter / 2.0) * SCALE_FACTOR);

    //Debug-Ausgabe, falls man die tatsächlichen Größen in Weltkoordinaten wissen will
    //qDebug() << "Skalierter Radius von" << _name << "beträgt" << ((_diameter / 2.0) * SCALE_FACTOR);
    //qDebug() << "Berechneter Abstand von" << _name << "zur Sonne =" << (_orbitalRadius * SCALE_FACTOR);
}

