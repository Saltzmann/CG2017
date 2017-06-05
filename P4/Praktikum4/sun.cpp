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
    _secondaryTexture = nullptr;
    _setSecondaryTexture(secondaryTextureFileName);

    _distortionCounter = 0.f;
}

void Sun::RenderWithChildren(QMatrix4x4 ctm,
                        QMatrix4x4 const &viewMatrix,
                        QMatrix4x4 const &projectionMatrix,
                        unsigned int const &iboLength,
                        unsigned short const &vertOffset,
                        unsigned short const &normOffset,
                        unsigned short const &texCoordOffset,
                        size_t const &stride,
                        bool const &hasTextureCoords) {
    //qDebug() << "RenderWithChildren: Sun";
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
    _secondaryTexture->bind(1);

    //an shader übergeben
    _shader->setUniformValue("diffuseMap", 0);
    _shader->setUniformValue("distortionMap", 1);

    //zeichnen lassen
    glDrawElements(GL_TRIANGLES, iboLength, GL_UNSIGNED_INT, 0);

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
    _secondaryTexture->release();

    _distortionCounter += 0.0002;
}

void Sun::_setSecondaryTexture(QString filename) {
    _secondaryTexture = new QOpenGLTexture(QImage(":/" + filename).mirrored());
    _secondaryTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _secondaryTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    _secondaryTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
    // Anm.: Wenn qTex->textureId() == 0 ist, dann ist etwas schief gegangen
    Q_ASSERT(_secondaryTexture->textureId() != 0);
}
