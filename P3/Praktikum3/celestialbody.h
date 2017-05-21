#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#include <QOpenGLTexture>
#include <QVector>
#include <QMatrix4x4>


class CelestialBody
{
private:
    //Atrributes
    //initial
    double _diameter;
    float _axialTilt;
    float _rotationPeriod;
    double _orbitalPeriod;
    double _orbitalRadius;
    bool _CCWRotation;
    //general
    CelestialBody* _inOrbitOf;
    QOpenGLTexture* _qTex;
    //fixed calculated values (for performance)
    float _rotationalAnglePerDay;
    float _orbitalAnglePerDay;
    //current location & rotation
    float _currentOrbitalAngle;
    float _currentRotationalAngle;

    //methods
    double _getScale();
    void _setTexture(QString filename);
public:
    CelestialBody(double diameter, float axialTilt,
           float rotationPeriod, float orbitalPeriod,
           double orbitalRadius, bool CCWRotation,
           QString textureFileName,
           CelestialBody* inOrbitOf = nullptr); //if moon
    //getters for initial attributes 'cause y not
    double getDiameter();
    float getAxialTilt();
    float getRotationPeriod();
    double getOrbitalPeriod();
    double getOrbitalRadius();
    //getter for calculated attributes 'cause y not
    float getRotationalAnglePerDay();
    float getOrbitalAnglePerDay();

    void update();
    QOpenGLTexture* getTexture();

    QMatrix4x4 getTransformationMatrix();
    bool isInOrbitOf();
};

#endif // CELESTIALBODY_H
