
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/QOrbitCameraController>

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

class CameraController : public QObject {

    Q_OBJECT

  public:
    explicit CameraController( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : m_rootEntity( rootEntity ), m_cameraEntity( cameraEntity ), m_orbitController( nullptr ), m_firstPersonController( nullptr ) {
    }

    ~CameraController() {}

  public slots:
    void setCameraController( int camMode ) {
      switch( camMode ) {
        case 0: {
            if( m_orbitController != nullptr ) {
              delete m_orbitController;
              m_orbitController = nullptr;
            }

            if( m_firstPersonController != nullptr ) {
              delete m_firstPersonController;
              m_firstPersonController = nullptr;
            }
          }
          break;

        case 1: {
            if( m_orbitController != nullptr ) {
              delete m_orbitController;
              m_orbitController = nullptr;
            }

            if( m_firstPersonController == nullptr ) {
              m_firstPersonController = new Qt3DExtras::QFirstPersonCameraController( m_rootEntity );
              m_firstPersonController->setCamera( m_cameraEntity );
              m_firstPersonController->setLinearSpeed( 150 );
            }
          }
          break;

        default: {
            if( m_firstPersonController != nullptr ) {
              delete m_firstPersonController;
              m_firstPersonController = nullptr;
            }

            if( m_orbitController == nullptr ) {
              m_orbitController = new Qt3DExtras::QOrbitCameraController( m_rootEntity );
              m_orbitController->setCamera( m_cameraEntity );
              m_orbitController->setLinearSpeed( 150 );
            }
          }
          break;
      }
    }

  private:
    Qt3DCore::QEntity* m_rootEntity;
    Qt3DRender::QCamera* m_cameraEntity;

    Qt3DExtras::QOrbitCameraController* m_orbitController;
    Qt3DExtras::QFirstPersonCameraController* m_firstPersonController;

};

#endif // CAMERACONTROLLER_H
