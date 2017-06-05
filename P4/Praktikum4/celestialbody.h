#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#define TICKS_PER_SECOND 60
#define SIMYEARS_PER_TICK 0.005/(double)TICKS_PER_SECOND
#define SCALE_FACTOR 1/4879.4 //vielfaches von Merkur


#include <QOpenGLTexture>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>


class CelestialBody : public QObject
{
    Q_OBJECT
protected:
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
    QOpenGLTexture* _mainTexture;
    QOpenGLTexture* _secondaryTexture;
    QOpenGLShaderProgram* _shader;
    //fixed calculated values (for performance)
    float _rotationalAnglePerTick;
    float _orbitalAnglePerTick;
    //current location & rotation
    float _currentOrbitalAngle;
    float _currentRotationalAngle;

    //methods
    void _setMainTexture(QString filename);
    void _setSecondaryTexture(QString filename);
    void _getOrbitalTransformationMatrix(QMatrix4x4 &matrix);
    void _getRotationTransformationMatrix(QMatrix4x4 &matrix);
public:
    CelestialBody(QString planetName,
                  double diameter, float axialTilt,
                  float rotationPeriod, float orbitalPeriod,
                  double orbitalRadius, QString mainTextureFileName,
                  QOpenGLShaderProgram* myShader,
                  QString secondaryTextureFileName = "");
    void addOrbitingCelestialBody(CelestialBody* child);
    bool hasCelestialBodiesOrbiting();
    virtual void RenderWithChildren(QMatrix4x4 ctm,
                            QMatrix4x4 const &viewMatrix,
                            QMatrix4x4 const &projectionMatrix,
                            unsigned int const &iboLength,
                            unsigned short const &vertOffset,
                            unsigned short const &normOffset,
                            unsigned short const &texCoordOffset,
                            size_t const &stride,
                            bool const &hasTextureCoords);
public slots:
   void update();
};

#endif // CELESTIALBODY_H
