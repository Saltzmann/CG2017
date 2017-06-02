#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#define TICKS_PER_SECOND 60
#define SIMYEARS_PER_TICK 0.05/(double)TICKS_PER_SECOND
#define SCALE_FACTOR 1/4879.4 //vielfaches von Merkur


#include <QOpenGLTexture>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>


class CelestialBody : public QObject
{
    Q_OBJECT
private:
    //Atrributes
    //initial
    QString _name;
    double _diameter;
    float _axialTilt;
    float _rotationPeriod;
    double _orbitalPeriod;
    double _orbitalRadius;
    //general
    QVector<CelestialBody*> Orbiting;
    QOpenGLTexture* _qTex;
    //fixed calculated values (for performance)
    float _rotationalAnglePerTick;
    float _orbitalAnglePerTick;
    //current location & rotation
    float _currentOrbitalAngle;
    float _currentRotationalAngle;

    //methods
    void _setTexture(QString filename);
    QMatrix4x4 _getOrbitalTransformationMatrix();
    QMatrix4x4 _getRotationTransformationMatrix();
public:
    CelestialBody(QString planetName,
                  double diameter, float axialTilt,
                  float rotationPeriod, float orbitalPeriod,
                  double orbitalRadius, QString textureFileName);
    void addOrbitingCelestialBody(CelestialBody* child);
    bool hasCelestialBodiesOrbiting();
    void RenderWithChildren(int const &attrVertices,
                            int const &attrTexCoords,
                            QMatrix4x4 ctm,
                            QOpenGLShaderProgram& shader,
                            unsigned int const &iboLength,
                            QVector3D const &viewingOffsets,
                            QVector3D const &viewingAngles);
public slots:
   void update();
};

#endif // CELESTIALBODY_H
