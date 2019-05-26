
// tractor-trailer kinematics
// https://userpages.uni-koblenz.de/~zoebel/pdf/Mamotep2.pdf
// http://correll.cs.colorado.edu/?p=1869
// https://github.com/correll/Introduction-to-Autonomous-Robots/releases

// axis/rotation conventions
// https://ch.mathworks.com/help/driving/ug/coordinate-systems.html

#include <QGuiApplication>

#include <Qt3DRender/QCamera>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCameraLens>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QSlider>
#include <QtGui/QScreen>

#include <Qt3DInput/QInputAspect>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QPointLight>

#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QOrbitCameraController>

#include "gui/settingsdialog.h"
#include "gui/guidancetoolbar.h"
#include "gui/simulatortoolbar.h"

#include "3d/cameracontroller.h"
#include "3d/TractorModel.h"
#include "3d/TrailerModel.h"

#include "PoseSimulation.h"
#include "PoseCache.h"

#include "kinematic/FixedKinematic.h"
#include "kinematic/TrailerKinematic.h"

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"

int main( int argc, char** argv ) {
  QApplication app( argc, argv );

  Qt3DExtras::Qt3DWindow* view = new Qt3DExtras::Qt3DWindow();
  view->defaultFrameGraph()->setClearColor( QColor( QRgb( 0x4d4d4f ) ) );
  QWidget* container = QWidget::createWindowContainer( view );
  QSize screenSize = view->screen()->size();
  container->setMinimumSize( QSize( 500, 400 ) );
  container->setMaximumSize( screenSize );

  QWidget* widget = new QWidget;
  QHBoxLayout* hLayout = new QHBoxLayout( widget );
  hLayout->addWidget( container, 1 );



  widget->setWindowTitle( QStringLiteral( "QtOpenGuidance" ) );

  Qt3DInput::QInputAspect* input = new Qt3DInput::QInputAspect;
  view->registerAspect( input );

  // Root entity
  Qt3DCore::QEntity* rootEntity = new Qt3DCore::QEntity();

  // Camera
  Qt3DRender::QCamera* cameraEntity = view->camera();

  cameraEntity->lens()->setPerspectiveProjection( 45.0f, 16.0f / 10.0f, 0.1f, 1000.0f );
  cameraEntity->setPosition( QVector3D( 0, 0, 20.0f ) );
  cameraEntity->setUpVector( QVector3D( 0, 1, 0 ) );
  cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );
  cameraEntity->rollAboutViewCenter( -90 );
  cameraEntity->tiltAboutViewCenter( -45 );

  Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity( rootEntity );
  Qt3DRender::QPointLight* light = new Qt3DRender::QPointLight( lightEntity );
  light->setColor( "white" );
  light->setIntensity( 1 );
  lightEntity->addComponent( light );
  Qt3DCore::QTransform* lightTransform = new Qt3DCore::QTransform( lightEntity );
  lightTransform->setTranslation( cameraEntity->position() );
  lightEntity->addComponent( lightTransform );

  // For camera controls
  CameraController* cameraController = new CameraController( rootEntity, cameraEntity );
  cameraController->setCameraController( 2 );

  //Qt3DExtras::QOrbitCameraController* camController = new Qt3DExtras::QOrbitCameraController( rootEntity );
  //camController->setCamera( cameraEntity );
  //camController->setLinearSpeed( 150 );

//  // TractorEntity
////  TractorModel* tractorModelSimulation = new TractorModel( rootEntity );
//  TractorModel* tractorModel = new TractorModel( rootEntity );
//  TrailerModel* trailerModel = new TrailerModel( rootEntity );
//  TrailerModel* trailerModel2 = new TrailerModel( rootEntity );
//  TrailerModel* trailerModel3 = new TrailerModel( rootEntity );

  // draw an axis-cross: X-red, Y-green, Z-blue
  if( 1 ) {
    Qt3DCore::QEntity* xaxis = new Qt3DCore::QEntity( rootEntity );
    Qt3DCore::QEntity* yaxis = new Qt3DCore::QEntity( rootEntity );
    Qt3DCore::QEntity* zaxis = new Qt3DCore::QEntity( rootEntity );

    Qt3DExtras::QCylinderMesh* cylinderMesh = new Qt3DExtras::QCylinderMesh();
    cylinderMesh->setRadius( 0.05 );
    cylinderMesh->setLength( 20 );
    cylinderMesh->setRings( 2 );
    cylinderMesh->setSlices( 4 );

    Qt3DExtras::QPhongMaterial* blueMaterial = new Qt3DExtras::QPhongMaterial();
    blueMaterial->setSpecular( Qt::white );
    blueMaterial->setShininess( 10 );
    blueMaterial->setAmbient( Qt::blue );

    Qt3DExtras::QPhongMaterial* redMaterial = new Qt3DExtras::QPhongMaterial();
    redMaterial->setSpecular( Qt::white );
    redMaterial->setShininess( 10 );
    redMaterial->setAmbient( Qt::red );

    Qt3DExtras::QPhongMaterial* greenMaterial = new Qt3DExtras::QPhongMaterial();
    greenMaterial->setSpecular( Qt::white );
    greenMaterial->setShininess( 10 );
    greenMaterial->setAmbient( Qt::green );

    Qt3DCore::QTransform* xTransform = new Qt3DCore::QTransform();
    xTransform->setTranslation( QVector3D( 10, 0, 0 ) );
    xTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), 90 ) );
    Qt3DCore::QTransform* yTransform = new Qt3DCore::QTransform();
    yTransform->setTranslation( QVector3D( 0, 10, 0 ) );
    Qt3DCore::QTransform* zTransform = new Qt3DCore::QTransform();
    zTransform->setTranslation( QVector3D( 0, 0, 10 ) );
    zTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 1, 0, 0 ), 90 ) );


    xaxis->addComponent( cylinderMesh );
    xaxis->addComponent( redMaterial );
    xaxis->addComponent( xTransform );
    yaxis->addComponent( cylinderMesh );
    yaxis->addComponent( greenMaterial );
    yaxis->addComponent( yTransform );
    zaxis->addComponent( cylinderMesh );
    zaxis->addComponent( blueMaterial );
    zaxis->addComponent( zTransform );
  }

  // Set root object of the scene
  view->setRootEntity( rootEntity );




  // Toolbars
  SimulatorToolbar* simulatorToolbar = new SimulatorToolbar( widget );
  simulatorToolbar->setVisible( false );
  hLayout->addWidget( simulatorToolbar );

  GuidanceToolbar* guidaceToolbar = new GuidanceToolbar( widget );
  hLayout->addWidget( guidaceToolbar );

  // Create setting Window
  SettingsDialog* settingDialog = new SettingsDialog( rootEntity, widget );


  // GUI -> GUI
  QObject::connect( guidaceToolbar, SIGNAL( simulatorChanged( bool ) ),
                    simulatorToolbar, SLOT( setVisible( bool ) ) );
  QObject::connect( guidaceToolbar, SIGNAL( toggleSettings() ),
                    settingDialog, SLOT( toggleVisibility() ) );


  // the processer of Pose etc
  GuidanceFactory* poseSimulationFactory = new PoseSimulationFactory();
  GuidanceBase* poseSimulation = poseSimulationFactory->createNewObject();
  poseSimulationFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView(), poseSimulation );

  //  PoseCache* poseCache = new PoseCache();

  //  FixedKinematic* tractorKinematics = new FixedKinematic();
  //  TrailerKinematic* trailerKinematics = new TrailerKinematic();
  //  TrailerKinematic* trailerKinematics2 = new TrailerKinematic();
  //  TrailerKinematic* trailerKinematics3 = new TrailerKinematic();

//  //  Settings -> Tractor Model
//  QObject::connect( settingDialog, SIGNAL( wheelbaseChanged( float ) ),
//                    tractorModel, SLOT( setWheelbase( float ) ) );

//  // Settings -> Tractor Kinematics
//  QObject::connect( settingDialog, SIGNAL( antennaPositionChanged( QVector3D ) ),
//                    tractorKinematics, SLOT( setOffsetHookPointPosition( QVector3D ) ) );
//  QObject::connect( settingDialog, SIGNAL( hitchPositionChanged( QVector3D ) ),
//                    tractorKinematics, SLOT( setOffsetTowPointPosition( QVector3D ) ) );


  // Simulator GUI -> Simulator
  QObject::connect( guidaceToolbar, SIGNAL( simulatorChanged( bool ) ),
                    poseSimulation, SLOT( setSimulation( bool ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( velocityChanged( float ) ),
                    poseSimulation, SLOT( setVelocity( float ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( frequencyChanged( int ) ),
                    poseSimulation, SLOT( setFrequency( int ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( steerangleChanged( float ) ),
                    poseSimulation, SLOT( setSteerAngle( float ) ) );

//  // Settings -> Simulator
//  QObject::connect( settingDialog, SIGNAL( wheelbaseChanged( float ) ),
//                    poseSimulation, SLOT( setWheelbase( float ) ) );
//  QObject::connect( settingDialog, SIGNAL( antennaPositionChanged( QVector3D ) ),
//                    poseSimulation, SLOT( setAntennaPosition( QVector3D ) ) );


//  // Simulator -> PoseCache
//  QObject::connect( poseSimulation, SIGNAL( positionChanged( QVector3D ) ),
//                    poseCache, SLOT( setPosition( QVector3D ) ) );
//  QObject::connect( poseSimulation, SIGNAL( orientationChanged( QQuaternion ) ),
//                    poseCache, SLOT( setOrientation( QQuaternion ) ) );
//  QObject::connect( poseSimulation, SIGNAL( steeringAngleChanged( float ) ),
//                    poseCache, SLOT( setSteeringAngle( float ) ) );


//  // PoseCache -> Tractor Kinematics
//  QObject::connect( poseCache, SIGNAL( poseChanged( QVector3D, QQuaternion ) ),
//                    tractorKinematics, SLOT( setPose( QVector3D, QQuaternion ) ) );

//  // Tractor Kinematics -> Trailer Kinematics
//  QObject::connect( tractorKinematics, SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ),
//                    trailerKinematics, SLOT( setPose( QVector3D, QQuaternion ) ) );

//  // Trailer Kinematics -> Trailer Kinematics
//  QObject::connect( trailerKinematics, SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ),
//                    trailerKinematics2, SLOT( setPose( QVector3D, QQuaternion ) ) );
//  QObject::connect( trailerKinematics2, SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ),
//                    trailerKinematics3, SLOT( setPose( QVector3D, QQuaternion ) ) );


//  // PoseCache -> Tractor Model
//  QObject::connect( poseCache, SIGNAL( steeringAngleChanged( float ) ),
//                    tractorModel, SLOT( setSteeringAngle( float ) ) );

//  // Tractor Kinematics -> Tractor Model
//  QObject::connect( tractorKinematics, SIGNAL( posePivotPointChanged( QVector3D, QQuaternion ) ),
//                    tractorModel, SLOT( setPosePivotPoint( QVector3D, QQuaternion ) ) );
//  QObject::connect( tractorKinematics, SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ),
//                    tractorModel, SLOT( setPoseTowPoint( QVector3D, QQuaternion ) ) );
//  QObject::connect( tractorKinematics, SIGNAL( poseHookPointChanged( QVector3D, QQuaternion ) ),
//                    tractorModel, SLOT( setPoseHookPoint( QVector3D, QQuaternion ) ) );

//  // Trailer Kinematics -> Trailer Model
//  QObject::connect( trailerKinematics, SIGNAL( posePivotPointChanged( QVector3D, QQuaternion ) ),
//                    trailerModel, SLOT( setPosePivotPoint( QVector3D, QQuaternion ) ) );
//  QObject::connect( trailerKinematics, SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ),
//                    trailerModel, SLOT( setPoseTowPoint( QVector3D, QQuaternion ) ) );
//  QObject::connect( trailerKinematics, SIGNAL( poseHookPointChanged( QVector3D, QQuaternion ) ),
//                    trailerModel, SLOT( setPoseHookPoint( QVector3D, QQuaternion ) ) );

//  // Trailer Kinematics -> Trailer Model
//  QObject::connect( trailerKinematics2, SIGNAL( posePivotPointChanged( QVector3D, QQuaternion ) ),
//                    trailerModel2, SLOT( setPosePivotPoint( QVector3D, QQuaternion ) ) );
//  QObject::connect( trailerKinematics2, SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ),
//                    trailerModel2, SLOT( setPoseTowPoint( QVector3D, QQuaternion ) ) );
//  QObject::connect( trailerKinematics2, SIGNAL( poseHookPointChanged( QVector3D, QQuaternion ) ),
//                    trailerModel2, SLOT( setPoseHookPoint( QVector3D, QQuaternion ) ) );
//  QObject::connect( trailerKinematics3, SIGNAL( posePivotPointChanged( QVector3D, QQuaternion ) ),
//                    trailerModel3, SLOT( setPosePivotPoint( QVector3D, QQuaternion ) ) );
//  QObject::connect( trailerKinematics3, SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ),
//                    trailerModel3, SLOT( setPoseTowPoint( QVector3D, QQuaternion ) ) );
//  QObject::connect( trailerKinematics3, SIGNAL( poseHookPointChanged( QVector3D, QQuaternion ) ),
//                    trailerModel3, SLOT( setPoseHookPoint( QVector3D, QQuaternion ) ) );


  // Camerastuff
  QObject::connect( settingDialog, SIGNAL( cameraChanged( int ) ),
                    cameraController, SLOT( setCameraController( int ) ) );

//  QObject::connect( poseSimulation, SIGNAL( positionChangedRelative( QVector3D ) ),
//                    cameraEntity, SLOT( translateWorld( QVector3D ) ) );
//  QObject::connect( poseSimulation, SIGNAL( positionChanged( QVector3D ) ),
//                    cameraEntity, SLOT( setViewCenter( QVector3D ) ) );

  QObject::connect( cameraEntity, SIGNAL( positionChanged( QVector3D ) ),
                    lightTransform, SLOT( setTranslation( QVector3D ) ) );

//  QObject::connect( poseSimulation, SIGNAL( orientationChangedRelative( QQuaternion ) ),
//                    cameraEntity, SLOT( rotateAboutViewCenter( QQuaternion ) ) );

//  // Simulation tractor
//  {
//    // PoseCache -> Tractor Model
//    QObject::connect( poseCache, SIGNAL( poseChanged( QVector3D, QQuaternion ) ),
//                      tractorModelSimulation, SLOT( setPosePivotPoint( QVector3D, QQuaternion ) ) );
//    QObject::connect( poseCache, SIGNAL( steeringAngleChanged( float ) ),
//                      tractorModelSimulation, SLOT( setSteeringAngle( float ) ) );
//    QObject::connect( settingDialog, SIGNAL( wheelbaseChanged( float ) ),
//                      tractorModelSimulation, SLOT( setWheelbase( float ) ) );
//  }

  // setup simulator and tractor kinematics
  {
//    poseSimulation->setAntennaPosition( QVector3D( 3, 0, 1.5 ) );
//    tractorKinematics->setOffsetHookPointPosition( QVector3D( 3, 0, 1.5 ) );
//    tractorKinematics->setOffsetTowPointPosition( QVector3D( -1, 0, 0 ) );

//    trailerKinematics->setOffsetHookPointPosition( QVector3D( 3, 0, 0 ) );
//    trailerKinematics->setOffsetTowPointPosition( QVector3D( -1, 0, 0 ) );
//    trailerModel->setOffsetHookPointPosition( QVector3D( 3, 0, 0 ) );

//    trailerKinematics2->setOffsetHookPointPosition( QVector3D( 6, 0, 0 ) );
//    trailerKinematics2->setOffsetTowPointPosition( QVector3D( -1, 0, 0 ) );
//    trailerModel2->setOffsetHookPointPosition( QVector3D( 6, 0, 0 ) );
//    trailerKinematics3->setOffsetHookPointPosition( QVector3D( 8, 0, 0 ) );
//    trailerKinematics3->setOffsetTowPointPosition( QVector3D( -1, 0, 0 ) );
//    trailerModel3->setOffsetHookPointPosition( QVector3D( 8, 0, 0 ) );

  }

  // Show window
  widget->show();
  widget->resize( 1200, 800 );

  return app.exec();
}

