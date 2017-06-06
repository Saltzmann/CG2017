#ifndef SUN_H
#define SUN_H

#include <QObject>
#include <celestialbody.h>

class Sun : public CelestialBody
{
private:
    unsigned int _distortionCounterX;
    //float _distortionCounterX;
    float _distortionCounterY;

    QOpenGLTexture* _secondaryTexture;
    void _setSecondaryTexture(QString filename);
    char _modHeavySideFunction(int xValue);
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
                            bool const &hasTextureCoords,
                            float const &passthru1,
                            float const &passthru2);
};

#endif // SUN_H
