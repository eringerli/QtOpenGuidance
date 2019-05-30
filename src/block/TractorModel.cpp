// Copyright( C ) 2019 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "TractorModel.h"

#include <QtCore/QDebug>
#include <QtMath>


TractorModel::TractorModel( Qt3DCore::QEntity* rootEntity )
  : GuidanceBase() {
  m_rootEntityTransform = new Qt3DCore::QTransform();

  // add an etry, so all coordinates are local
  m_rootEntity = new Qt3DCore::QEntity( rootEntity );
  m_rootEntity->addComponent( m_rootEntityTransform );

  // base
  {
    m_baseMesh = new Qt3DExtras::QCuboidMesh();
    m_baseTransform = new Qt3DCore::QTransform();

    auto* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x665423 ) ) );

    m_baseEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_baseEntity->addComponent( m_baseMesh );
    m_baseEntity->addComponent( material );
    m_baseEntity->addComponent( m_baseTransform );
  }

  m_wheelFrontMesh = new Qt3DExtras::QCylinderMesh();

  // wheel front left
  {
    m_wheelFrontLeftTransform = new Qt3DCore::QTransform();

    auto* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x668823 ) ) );

    m_wheelFrontLeftEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelFrontLeftEntity->addComponent( m_wheelFrontMesh );
    m_wheelFrontLeftEntity->addComponent( material );
    m_wheelFrontLeftEntity->addComponent( m_wheelFrontLeftTransform );
  }

  // wheel front right
  {
    m_wheelFrontRightTransform = new Qt3DCore::QTransform();

    auto* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x668823 ) ) );

    m_wheelFrontRightEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelFrontRightEntity->addComponent( m_wheelFrontMesh );
    m_wheelFrontRightEntity->addComponent( material );
    m_wheelFrontRightEntity->addComponent( m_wheelFrontRightTransform );
  }

  m_wheelBackMesh = new Qt3DExtras::QCylinderMesh();
  // wheel back left
  {
    m_wheelBackLeftTransform = new Qt3DCore::QTransform();

    auto* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x668823 ) ) );

    m_wheelBackLeftEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelBackLeftEntity->addComponent( m_wheelBackMesh );
    m_wheelBackLeftEntity->addComponent( material );
    m_wheelBackLeftEntity->addComponent( m_wheelBackLeftTransform );
  }

  // wheel back right
  {
    m_wheelBackRightTransform = new Qt3DCore::QTransform();

    auto* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( QRgb( 0x668823 ) ) );

    m_wheelBackRightEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelBackRightEntity->addComponent( m_wheelBackMesh );
    m_wheelBackRightEntity->addComponent( material );
    m_wheelBackRightEntity->addComponent( m_wheelBackRightTransform );
  }

  // most dimensions are dependent on the wheelbase -> the code for that is in the slot setWheelbase(float)
  setWheelbase( m_wheelbase );

  // everything in world-coordinates... (add to rootEntity, not m_rootEntity)
  {
    // tow hook marker -> red
    {
      m_towHookMesh = new Qt3DExtras::QSphereMesh();
      m_towHookMesh->setRadius( .2 );
      m_towHookMesh->setSlices( 20 );
      m_towHookMesh->setRings( 20 );

      m_towHookTransform = new Qt3DCore::QTransform();

      Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
      material->setDiffuse( QColor( QRgb( 0xaa3333 ) ) );

      m_towHookEntity = new Qt3DCore::QEntity( rootEntity );
      m_towHookEntity->addComponent( m_towHookMesh );
      m_towHookEntity->addComponent( material );
      m_towHookEntity->addComponent( m_towHookTransform );
    }

    // pivot point marker -> green
    {
      m_pivotPointMesh = new Qt3DExtras::QSphereMesh();
      m_pivotPointMesh->setRadius( .2 );
      m_pivotPointMesh->setSlices( 20 );
      m_pivotPointMesh->setRings( 20 );

      m_pivotPointTransform = new Qt3DCore::QTransform();

      Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
      material->setDiffuse( QColor( QRgb( 0x33aa33 ) ) );

      m_pivotPointEntity = new Qt3DCore::QEntity( rootEntity );
      m_pivotPointEntity->addComponent( m_pivotPointMesh );
      m_pivotPointEntity->addComponent( material );
      m_pivotPointEntity->addComponent( m_pivotPointTransform );
    }

    // hitch marker -> blue
    {
      m_towPointMesh = new Qt3DExtras::QSphereMesh();
      m_towPointMesh->setRadius( .2 );
      m_towPointMesh->setSlices( 20 );
      m_towPointMesh->setRings( 20 );

      m_towPointTransform = new Qt3DCore::QTransform();

      Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
      material->setDiffuse( QColor( QRgb( 0x3333aa ) ) );

      m_towPointEntity = new Qt3DCore::QEntity( rootEntity );
      m_towPointEntity->addComponent( m_towPointMesh );
      m_towPointEntity->addComponent( material );
      m_towPointEntity->addComponent( m_towPointTransform );
    }
  }

  // Axis
  if( false ) {
    auto* xaxis = new Qt3DCore::QEntity( m_rootEntity );
    auto* yaxis = new Qt3DCore::QEntity( m_rootEntity );
    auto* zaxis = new Qt3DCore::QEntity( m_rootEntity );

    auto* cylinderMesh = new Qt3DExtras::QCylinderMesh();
    cylinderMesh->setRadius( 0.2 );
    cylinderMesh->setLength( 20 );
    cylinderMesh->setRings( 100 );
    cylinderMesh->setSlices( 20 );

    auto* blueMaterial = new Qt3DExtras::QPhongMaterial();
    blueMaterial->setSpecular( Qt::white );
    blueMaterial->setShininess( 10 );
    blueMaterial->setAmbient( Qt::blue );

    auto* redMaterial = new Qt3DExtras::QPhongMaterial();
    redMaterial->setSpecular( Qt::white );
    redMaterial->setShininess( 10 );
    redMaterial->setAmbient( Qt::red );

    auto* greenMaterial = new Qt3DExtras::QPhongMaterial();
    greenMaterial->setSpecular( Qt::white );
    greenMaterial->setShininess( 10 );
    greenMaterial->setAmbient( Qt::green );

    auto* xTransform = new Qt3DCore::QTransform();
    xTransform->setTranslation( QVector3D( 10, 0, 0 ) );
    xTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), 90 ) );
    auto* yTransform = new Qt3DCore::QTransform();
    yTransform->setTranslation( QVector3D( 0, 10, 0 ) );
    auto* zTransform = new Qt3DCore::QTransform();
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

// order is important! Crashes if a parent entity is removed first!
TractorModel::~TractorModel() {
  delete  m_baseMesh;
  delete m_wheelFrontMesh;
  delete m_wheelBackMesh;

  delete  m_towHookMesh;
  delete m_pivotPointMesh;
  delete m_towPointMesh;

  delete m_rootEntityTransform;
  delete m_baseTransform;
  delete m_wheelFrontLeftTransform;
  delete m_wheelFrontRightTransform;
  delete m_wheelBackLeftTransform;
  delete m_wheelBackRightTransform;

  delete m_towHookTransform;
  delete m_pivotPointTransform;
  delete m_towPointTransform;


  delete m_towHookEntity;
  delete m_pivotPointEntity;
  delete m_towPointEntity;

  delete m_wheelFrontLeftEntity;
  delete m_wheelFrontRightEntity;
  delete m_wheelBackLeftEntity;
  delete m_wheelBackRightEntity;
  delete m_baseEntity;

  delete m_rootEntity;
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

  // wheels front
  {
    m_wheelFrontMesh->setRadius( wheelbase / 4 );
    m_wheelFrontMesh->setLength( wheelbase / 5 );
    m_wheelFrontLeftTransform->setTranslation( QVector3D( wheelbase, offsetForWheels + m_wheelFrontMesh->length() / 2, m_wheelFrontMesh->radius() ) );
    m_wheelFrontRightTransform->setTranslation( QVector3D( wheelbase, -( offsetForWheels + m_wheelFrontMesh->length() / 2 ),  m_wheelFrontMesh->radius() ) );
  }

  // wheels back
  {
    m_wheelBackMesh->setRadius( wheelbase / 2 );
    m_wheelBackMesh->setLength( wheelbase / 5 );
    m_wheelBackLeftTransform->setTranslation( QVector3D( 0, offsetForWheels + m_wheelBackMesh->length() / 2,  m_wheelBackMesh->radius() ) );
    m_wheelBackRightTransform->setTranslation( QVector3D( 0, -( offsetForWheels + m_wheelBackMesh->length() / 2 ),  m_wheelBackMesh->radius() ) );
  }

  setSteeringAngle( m_steeringAngle );
}

void TractorModel::setPoseTowPoint( QVector3D position, QQuaternion ) {
  m_towPointTransform->setTranslation( position );
}

void TractorModel::setPoseHookPoint( QVector3D position, QQuaternion ) {
  m_towHookTransform->setTranslation( position );
}


QVector3D TractorModel::position() {
  return m_rootEntityTransform->translation();
}

QQuaternion TractorModel::rotation() {
  return m_rootEntityTransform->rotation();
}

void TractorModel::setPosePivotPoint( QVector3D position, QQuaternion rotation ) {
  m_pivotPointTransform->setTranslation( position );

  m_rootEntityTransform->setTranslation( position );
  m_rootEntityTransform->setRotation( rotation );
  emit positionChanged( position );
  emit rotationChanged( rotation );
}

// http://correll.cs.colorado.edu/?p=1869
void TractorModel::setSteeringAngle( float steerAngle ) {
  m_steeringAngle = steerAngle;

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

    QQuaternion rotationLeft =  QQuaternion::fromAxisAndAngle( QVector3D( 0.0F, 0.0F, 1.0F ), qRadiansToDegrees( steeringAngleLeft ) );
    m_wheelFrontLeftTransform->setRotation( rotationLeft );

    QQuaternion rotationRight =  QQuaternion::fromAxisAndAngle( QVector3D( 0.0F, 0.0F, 1.0F ), qRadiansToDegrees( steeringAngleRight ) );
    m_wheelFrontRightTransform->setRotation( rotationRight );
  } else {
    m_wheelFrontLeftTransform->setRotation( QQuaternion() );
    m_wheelFrontRightTransform->setRotation( QQuaternion() );
  }
}
