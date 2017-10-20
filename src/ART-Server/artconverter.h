#ifndef CONVERTER_H
#define CONVERTER_H

#include <QObject>
#include <QtGui/QGenericMatrix>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtGui/QMatrix4x4>

class ARTConverter : public QObject
{
    Q_OBJECT
  public:
    explicit ARTConverter(QObject *parent = 0);
    ~ARTConverter();

    static QVector3D matrixToEuler(QMatrix4x4 matrix);
    static QMatrix4x4 eulerToMatrix(QVector3D euler);
    static QQuaternion matrixToQuaternion(QMatrix4x4 matrix);
    static QMatrix4x4 quaternionToMatrix(QQuaternion quaternion);
    static QVector3D quaternionToEuler(QQuaternion quat);
    static QQuaternion eulerToQuaternion(QVector3D euler);
    static QMatrix3x3 leftToRightHanded(QMatrix3x3 left);

    static QMatrix4x4 invertAxis(QVector4D normals, QMatrix4x4 matrix);
    static QMatrix4x4 switchAxis(QQuaternion axis, QMatrix4x4 matrix);

    static QString matrixToString(QMatrix3x3 matrix);
    static QMatrix3x3 stringToMatrix(QString string);

  signals:

  public slots:
};

#endif // CONVERTER_H
