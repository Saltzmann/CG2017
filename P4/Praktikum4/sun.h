#ifndef SUN_H
#define SUN_H

#include <QObject>
#include <celestialbody.h>

class Sun : public CelestialBody
{
private:
    //Attribute für Hitzeflimmern-Effekt
    unsigned int _distortionCounterX;
    //float _distortionCounterX; //für Sinus-Version
    float _distortionCounterY;

    //Erweiterung von CelestialBody um zweite Textur
    QOpenGLTexture* _secondaryTexture;
    void _setSecondaryTexture(QString filename);

    //Methode für Hitzeflackern-Effekt
    char _modHeavySideFunction(int xValue);
public:
    //Angepasster Kontruktor für zweite Textur
    Sun(QString planetName,
        double diameter, float axialTilt,
        float rotationPeriod, float orbitalPeriod,
        double orbitalRadius, QString mainTextureFileName,
        QOpenGLShaderProgram* myShader,
        QString secondaryTextureFileName);

    //Angepasste Renderfunktion
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
    //Anmerkung:
    //(update Funktion wird vererbt)
};

#endif // SUN_H
