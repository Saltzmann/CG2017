#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#define TICKS_PER_SECOND 60
#define SIMYEARS_PER_TICK 0.01/(double)TICKS_PER_SECOND


#include <QOpenGLTexture>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <stack>


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
    QVector<CelestialBody> Orbiting;
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
    QMatrix4x4 _getOrbitalTransformationMatrix();
    QMatrix4x4 _getRotationTransformationMatrix();
public:
    CelestialBody(double diameter, float axialTilt,
           float rotationPeriod, float orbitalPeriod,
           double orbitalRadius, bool CCWRotation,
           QString textureFileName);
    void addOrbitingCelestialBody(CelestialBody child);
    bool hasCelestialBodiesOrbiting();
    void RenderWithChildren(QOpenGLShaderProgram* shader,
                std::stack<QMatrix4x4> matrixStack,
                GLfloat* vboData,
                GLuint* indexData);
public slots:
   void update();
};

#endif // CELESTIALBODY_H
