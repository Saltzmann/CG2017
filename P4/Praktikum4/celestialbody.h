#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#define TICKS_PER_SECOND 60
#define SIMYEARS_PER_TICK 0.001/(double)TICKS_PER_SECOND
#define SCALE_FACTOR 1/4879.4 //vielfaches von Merkur
//#define SKYBOX_DIAMETER 25000000 //große Skybox
#define SKYBOX_DIAMETER 10000000 //kleinere Skybox

#include <QOpenGLTexture>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>


class CelestialBody : public QObject
{
    Q_OBJECT
protected:
    //initiale (dem Konstruktor übergebene) Attribute
    QString _name;
    double _diameter;
    float _axialTilt;
    float _rotationPeriod;
    double _orbitalPeriod;
    double _orbitalRadius;
    QOpenGLTexture* _mainTexture;
    QOpenGLShaderProgram* _shader;
    //Vector an Kind-Himmelskörpern
    QVector<CelestialBody*> Orbiting;
    //fest berechnete Attribute (für die Performanz)
    float _rotationalAnglePerTick;
    float _orbitalAnglePerTick;
    //aktuelle Transformationswerte
    float _currentOrbitalAngle;
    float _currentRotationalAngle;
    //private (ausgelagerte) Hilfsmethoden
    void _setMainTexture(QString filename);
    void _getOrbitalTransformationMatrix(QMatrix4x4 &matrix);
    void _getRotationTransformationMatrix(QMatrix4x4 &matrix);
public:
    //Konstruktor
    CelestialBody(QString planetName,
                  double diameter, float axialTilt,
                  float rotationPeriod, float orbitalPeriod,
                  double orbitalRadius, QString mainTextureFileName,
                  QOpenGLShaderProgram* myShader);
    //Funktion zum Aufbauen des Renderbaums
    void addOrbitingCelestialBody(CelestialBody* child);
    //Basisklassen-Methode zum Rendern des Himmelskörpers
    virtual void RenderWithChildren(QMatrix4x4 ctm,
                            QMatrix4x4 const &viewMatrix,
                            QMatrix4x4 const &projectionMatrix,
                            unsigned int const &iboLength,
                            unsigned short const &vertOffset,
                            unsigned short const &normOffset,
                            unsigned short const &texCoordOffset,
                            size_t const &stride,
                            bool const &hasTextureCoords,
                            float const &linMod,
                            float const &expMod);
public slots:
   //Update Slot um synchronisierte Timer-Updates für Rotation & Orbit zu bekommen
   void update();
};

#endif // CELESTIALBODY_H
