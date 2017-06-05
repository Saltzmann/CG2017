#ifndef SUN_H
#define SUN_H

#include <QObject>
#include <celestialbody.h>

class Sun : public CelestialBody
{
private:
    float _distortionCounter;
    QOpenGLTexture* _secondaryTexture;
    void _setSecondaryTexture(QString filename);
public:
    Sun(QString planetName,
        double diameter, float axialTilt,
        float rotationPeriod, float orbitalPeriod,
        double orbitalRadius, QString mainTextureFileName,
        QOpenGLShaderProgram* myShader,
        QString secondaryTextureFileName = "");

    virtual void RenderWithChildren(QMatrix4x4 ctm,
                            QMatrix4x4 const &viewMatrix,
                            QMatrix4x4 const &projectionMatrix,
                            unsigned int const &iboLength,
                            unsigned short const &vertOffset,
                            unsigned short const &normOffset,
                            unsigned short const &texCoordOffset,
                            size_t const &stride,
                            bool const &hasTextureCoords);
};

#endif // SUN_H
