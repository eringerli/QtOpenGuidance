
#ifndef POSECACHE_H
#define POSECACHE_H

#include <QtCore/QObject>

#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>

class PoseCache : public QObject {
    Q_OBJECT
    Q_PROPERTY( QVector3D position READ position WRITE setPosition NOTIFY poseChanged )
    Q_PROPERTY( QQuaternion orientation READ orientation WRITE setOrientation )

  public:
    explicit PoseCache()
      : m_position(), m_orientation() {}
    ~PoseCache() {}

    QVector3D position() {
      return m_position;
    }

    QQuaternion orientation() {
      return m_orientation;
    }

    float steeringAngle() {
      return m_steeringAngle;
    }

  public slots:
    void setPosition( QVector3D position ) {
      m_position = position;
//       qDebug() << "setPosition( QVector3D position ): " << position;

      emit poseChanged( m_position, m_orientation );
    }

    void setOrientation( QQuaternion orientation ) {
      m_orientation = orientation;
    }

    void setSteeringAngle( float steeringAngle ) {
      m_steeringAngle = steeringAngle;
      emit steeringAngleChanged( steeringAngle );
    }

  signals:
    void poseChanged( QVector3D position, QQuaternion orientation );
    void steeringAngleChanged( float );

  private:
    float m_steeringAngle;
    QVector3D m_position;
    QQuaternion m_orientation;
};

#endif // POSECACHE_H

