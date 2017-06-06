#include "sun.h"

Sun::Sun(QString planetName,
         double diameter, float axialTilt,
         float rotationPeriod, float orbitalPeriod,
         double orbitalRadius, QString mainTextureFileName,
         QOpenGLShaderProgram *myShader,
         QString secondaryTextureFileName) : CelestialBody(planetName,
                                                           diameter,
                                                           axialTilt,
                                                           rotationPeriod,
                                                           orbitalPeriod,
                                                           orbitalRadius,
                                                           mainTextureFileName,
                                                           myShader) {
    //Nach ausgeführtem Konstruktor von CelestialBody:
    //Zweit-Textur einbinden
    _secondaryTexture = nullptr;
    _setSecondaryTexture(secondaryTextureFileName);

    //Werte für Hitzeflimmern-Effekt initialiseren
    _distortionCounterX = 0;
    //_distortionCounterX = 0.f; //für Sinus-Version
    _distortionCounterY = 0.f;
}

void Sun::RenderWithChildren(QMatrix4x4 ctm,
                        QMatrix4x4 const &viewMatrix,
                        QMatrix4x4 const &projectionMatrix,
                        unsigned int const &iboLength,
                        unsigned short const &vertOffset,
                        unsigned short const &normOffset,
                        unsigned short const &texCoordOffset,
                        size_t const &stride,
                        bool const &hasTextureCoords,
                        float const &passthru1, //passthru1 und passthru2 werden nur weitergeleitet an CelBod Kindknoten mit phong shader
                        float const &passthru2) {
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
                                  passthru1,
                                  passthru2);
        }
    }

    //Nach diesem Aufruf beinhaltet ctm auch die Eigenrotation
    _getRotationTransformationMatrix(ctm);

    //Matrix Locations für den Shader
    int unifProjMatrix = 0;
    int unifViewMatrix = 1;
    int unifModelMatrix = 2;

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

    //Haupt- und Zweit-Textur binden und an den shader übergeben
    _mainTexture->bind(0);
    _secondaryTexture->bind(1);
    _shader->setUniformValue("diffuseMap", 0);
    _shader->setUniformValue("distortionMap", 1);

    //Für Hitzeflimmern-Effekt benötigte Uniforms an shader übergeben
    _shader->setUniformValue("timeX", float(0.001f * _modHeavySideFunction(_distortionCounterX)));
    //_shader->setUniformValue("timeX", float(0.001f * sin(_distortionCounterX))); //sinus-Version
    _shader->setUniformValue("timeY", _distortionCounterY);

    //Sonne zeichnen lassen
    glDrawElements(GL_TRIANGLES, iboLength, GL_UNSIGNED_INT, 0);

    // Deaktiviere die Verwendung der Attribute-Arrays
    _shader->disableAttributeArray(attrVertices);
    _shader->disableAttributeArray(attrNorms);
    if(hasTextureCoords) {
        _shader->disableAttributeArray(attrTexCoords);
    }

    //Shader lösen
    _shader->release();

    //Haupt- und Zweit-Textur aus dem OpenGL-Kontext lösen
    _mainTexture->release();
    _secondaryTexture->release();

    //Attribute für den nächsten Durchlauf für den Hitzeflimmer-Effekt anpassen
    _distortionCounterX += 1;
    //_distortionCounterX += 2; //für sinus-Version
    _distortionCounterY += 0.0005;
}

void Sun::_setSecondaryTexture(QString filename) {
    //(ausgelagerte) Hilfsfunktion zum Setzen der Zweit-Textur
    _secondaryTexture = new QOpenGLTexture(QImage(":/" + filename).mirrored());
    _secondaryTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _secondaryTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    _secondaryTexture->setWrapMode(QOpenGLTexture::Repeat);
    Q_ASSERT(_secondaryTexture->textureId() != 0); //Würde Fehler bedeuten
}

char Sun::_modHeavySideFunction(int xValue) {
    //Modifizierte Heavy-Side Funktion die zwischen 1 und -1 osziliert und nicht 1 und 0
    if(xValue % 2 == 0) {
        return 1;
    }
    return -1;
}
