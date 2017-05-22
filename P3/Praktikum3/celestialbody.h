#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#define TICKS_PER_SECOND 60
#define SIMYEARS_PER_TICK 0.01/(double)TICKS_PER_SECOND


#include <QOpenGLTexture>
#include <QVector>
#include <QMatrix4x4>


class CelestialBody : public QObject
{
    Q_OBJECT
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
    float _rotationalAnglePerTick;
    float _orbitalAnglePerTick;
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
    float getRotationalAnglePerTick();
    float getOrbitalAnglePerTick();

    QOpenGLTexture* getTexture();

    QMatrix4x4 getTransformationMatrix();
    bool isInOrbitOf();
public slots:
   void update();
};

#endif // CELESTIALBODY_H
