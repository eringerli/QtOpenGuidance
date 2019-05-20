
#include "TractorModel.h"

#include <QtCore/QDebug>
#include <QtMath>


TractorModel::TractorModel( Qt3DCore::QEntity* rootEntity )
  : m_hitchPosition(),
    m_wheelbase( 2.4 ),
    m_antennaPosition( 3, 0, 1.5 ) {

  m_rootEntityTransform = new Qt3DCore::QTransform();
  m_rootEntityTransform->setTranslation( QVector3D( 0, 0, 0 ) );
  m_rootEntityTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0.0f, 0.0f, 1.0f ), 0 ) );

  m_rootEntity = new Qt3DCore::QEntity( rootEntity );
  m_rootEntity->addComponent( m_rootEntityTransform );

  // base
  {
    m_baseMesh = new Qt3DExtras::QCuboidMesh();
    m_baseTransform = new Qt3DCore::QTransform();

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x665423 ) ) );

    m_baseEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_baseEntity->addComponent( m_baseMesh );
    m_baseEntity->addComponent( material );
    m_baseEntity->addComponent( m_baseTransform );
  }

  // wheel front left
  {
    m_wheelFrontLeftMesh = new Qt3DExtras::QCylinderMesh();
    m_wheelFrontLeftTransform = new Qt3DCore::QTransform();

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x668823 ) ) );

    m_wheelFrontLeftEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelFrontLeftEntity->addComponent( m_wheelFrontLeftMesh );
    m_wheelFrontLeftEntity->addComponent( material );
    m_wheelFrontLeftEntity->addComponent( m_wheelFrontLeftTransform );
  }

  // wheel front right
  {
    m_wheelFrontRightMesh = new Qt3DExtras::QCylinderMesh();
    m_wheelFrontRightTransform = new Qt3DCore::QTransform();

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x668823 ) ) );

    m_wheelFrontRightEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelFrontRightEntity->addComponent( m_wheelFrontRightMesh );
    m_wheelFrontRightEntity->addComponent( material );
    m_wheelFrontRightEntity->addComponent( m_wheelFrontRightTransform );
  }

  // wheel back left
  {
    m_wheelBackLeftMesh = new Qt3DExtras::QCylinderMesh();
    m_wheelBackLeftTransform = new Qt3DCore::QTransform();

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x668823 ) ) );

    m_wheelBackLeftEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelBackLeftEntity->addComponent( m_wheelBackLeftMesh );
    m_wheelBackLeftEntity->addComponent( material );
    m_wheelBackLeftEntity->addComponent( m_wheelBackLeftTransform );
  }

  // wheel back right
  {
    m_wheelBackRightMesh = new Qt3DExtras::QCylinderMesh();
    m_wheelBackRightTransform = new Qt3DCore::QTransform();

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x668823 ) ) );

    m_wheelBackRightEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelBackRightEntity->addComponent( m_wheelBackRightMesh );
    m_wheelBackRightEntity->addComponent( material );
    m_wheelBackRightEntity->addComponent( m_wheelBackRightTransform );
  }

  // most dimensions are dependent on the wheelbase -> the code for that is in the slot setWheelbase(float)
  setWheelbase( m_wheelbase );

  // antenna marker
  {
    m_antennaMesh = new Qt3DExtras::QSphereMesh();
    m_antennaMesh->setRadius( .2 );
    m_antennaMesh->setSlices( 20 );
    m_antennaMesh->setRings( 20 );

    m_antennaTransform = new Qt3DCore::QTransform();
    m_antennaTransform->setTranslation( m_antennaPosition );

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0xaa3333 ) ) );

    m_antennaEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_antennaEntity->addComponent( m_antennaMesh );
    m_antennaEntity->addComponent( material );
    m_antennaEntity->addComponent( m_antennaTransform );
  }

  // pivot point marker
  {
    m_pivotPointMesh = new Qt3DExtras::QSphereMesh();
    m_pivotPointMesh->setRadius( .2 );
    m_pivotPointMesh->setSlices( 20 );
    m_pivotPointMesh->setRings( 20 );

    m_pivotPointTransform = new Qt3DCore::QTransform();

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x33aa33 ) ) );

    m_pivotPointEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_pivotPointEntity->addComponent( m_pivotPointMesh );
    m_pivotPointEntity->addComponent( material );
    m_pivotPointEntity->addComponent( m_pivotPointTransform );
  }

  // hitch marker
  {
    m_hitchMesh = new Qt3DExtras::QSphereMesh();
    m_hitchMesh->setRadius( .2 );
    m_hitchMesh->setSlices( 20 );
    m_hitchMesh->setRings( 20 );

    m_hitchTransform = new Qt3DCore::QTransform();
    m_hitchTransform->setTranslation( m_hitchPosition );

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x3333aa ) ) );

    m_hitchEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_hitchEntity->addComponent( m_hitchMesh );
    m_hitchEntity->addComponent( material );
    m_hitchEntity->addComponent( m_hitchTransform );
  }

  // Axis
  if( 0 ) {
    Qt3DCore::QEntity* xaxis = new Qt3DCore::QEntity( m_rootEntity );
    Qt3DCore::QEntity* yaxis = new Qt3DCore::QEntity( m_rootEntity );
    Qt3DCore::QEntity* zaxis = new Qt3DCore::QEntity( m_rootEntity );

    Qt3DExtras::QCylinderMesh* cylinderMesh = new Qt3DExtras::QCylinderMesh();
    cylinderMesh->setRadius( 0.2 );
    cylinderMesh->setLength( 20 );
    cylinderMesh->setRings( 100 );
    cylinderMesh->setSlices( 20 );

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

}

TractorModel::~TractorModel() {
}

void TractorModel::setWheelbase( float wheelbase ) {

  m_wheelbase = wheelbase;

  float offsetForWheels = wheelbase / 4 * 3 / 2 + .1;

  // base
  {
    m_baseMesh->setYExtent( wheelbase / 4 * 3 );
    m_baseMesh->setXExtent( wheelbase );
    m_baseMesh->setZExtent( wheelbase / 4 );

    m_baseTransform->setTranslation( QVector3D( m_baseMesh->xExtent() / 2, 0, m_baseMesh->zExtent() / 2 + .25 ) );
  }

  // wheel front left
  {
    m_wheelFrontLeftMesh->setRadius( wheelbase / 4 );
    m_wheelFrontLeftMesh->setLength( wheelbase / 5 );

    m_wheelFrontLeftTransform->setTranslation( QVector3D( wheelbase, offsetForWheels + m_wheelFrontLeftMesh->length() / 2, m_wheelFrontLeftMesh->radius() ) );
  }

  // wheel front right
  {
    m_wheelFrontRightMesh->setRadius( wheelbase / 4 );
    m_wheelFrontRightMesh->setLength( wheelbase / 5 );

    m_wheelFrontRightTransform->setTranslation( QVector3D( wheelbase, -( offsetForWheels + m_wheelFrontRightMesh->length() / 2 ),  m_wheelFrontRightMesh->radius() ) );
  }

  // wheel back left
  {
    m_wheelBackLeftMesh->setRadius( wheelbase / 2 );
    m_wheelBackLeftMesh->setLength( wheelbase / 5 );

    m_wheelBackLeftTransform->setTranslation( QVector3D( 0, offsetForWheels + m_wheelBackLeftMesh->length() / 2,  m_wheelBackLeftMesh->radius() ) );
  }

  // wheel back right
  {
    m_wheelBackRightMesh->setRadius( wheelbase / 2 );
    m_wheelBackRightMesh->setLength( wheelbase / 5 );

    m_wheelBackRightTransform->setTranslation( QVector3D( 0, -( offsetForWheels + m_wheelBackRightMesh->length() / 2 ),  m_wheelBackRightMesh->radius() ) );
  }
  setSteerAngle( m_steerangle );
}

void TractorModel::setHitchPosition( QVector3D position ) {
  m_hitchPosition = position;
  m_hitchTransform->setTranslation( position);
}

void TractorModel::setAntennaPosition( QVector3D position ) {
  m_antennaPosition = position;
  m_antennaTransform->setTranslation( m_antennaPosition );
}

void TractorModel::setPosition( QVector3D position ) {
  m_rootEntityTransform->setTranslation( position );
  emit positionChanged( position );
}

QVector3D TractorModel::position() {
  return m_rootEntityTransform->translation();
}

void TractorModel::setRotation( QQuaternion rotation ) {
  m_rootEntityTransform->setRotation( rotation );
  emit rotationChanged( rotation );
}

QQuaternion TractorModel::rotation() {
  return m_rootEntityTransform->rotation();
}

void TractorModel::setPose( QVector3D position, QQuaternion rotation, float steerAngle ) {
    setSteerAngle(steerAngle);
  setPosition( position );
  setRotation( rotation );
}

// http://correll.cs.colorado.edu/?p=1869
void TractorModel::setSteerAngle( float steerAngle ) {
  m_steerangle = steerAngle;

  if( steerAngle != 0 ) {
    float distanceBetweenFrontWheels = m_wheelbase / 4 * 3  + .2 + m_wheelbase / 5;
    float radiusVirtualCurve = m_wheelbase / qTan( qDegreesToRadians( steerAngle ) );

    float steeringAngleLeft = qAtan( m_wheelbase / ( radiusVirtualCurve - ( distanceBetweenFrontWheels / 2 ) ) );
    float steeringAngleRight = qAtan( m_wheelbase / ( radiusVirtualCurve + ( distanceBetweenFrontWheels / 2 ) ) );

    // some trickery here: instead of turning the orientation later, do it in radians. the 'official' formala calls for -=M_PI_2 and *=-1
    // steeringAngleLeft -= M_PI_2;
    // steeringAngleRight -= M_PI_2;
    // steeringAngleLeft *= -1;
    // steeringAngleRight *= -1;
    steeringAngleLeft -= M_PI;
    steeringAngleRight -= M_PI;

    QQuaternion rotationLeft =  QQuaternion::fromAxisAndAngle( QVector3D( 0.0f, 0.0f, 1.0f ), qRadiansToDegrees( steeringAngleLeft ) );
    m_wheelFrontLeftTransform->setRotation( rotationLeft );

    QQuaternion rotationRight =  QQuaternion::fromAxisAndAngle( QVector3D( 0.0f, 0.0f, 1.0f ), qRadiansToDegrees( steeringAngleRight ) );
    m_wheelFrontRightTransform->setRotation( rotationRight );
  } else {
    m_wheelFrontLeftTransform->setRotation( QQuaternion() );
    m_wheelFrontRightTransform->setRotation( QQuaternion() );
  }
}
