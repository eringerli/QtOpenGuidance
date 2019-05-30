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

#include "TrailerModel.h"

#include <QtCore/QDebug>
#include <QtMath>


TrailerModel::TrailerModel( Qt3DCore::QEntity* rootEntity )
  : m_wheelbase( 2.4 ), m_offsetHookPoint( QVector3D( 6, 0, 0 ) ) {
  m_id = getNextUserId();

  m_rootEntityTransform = new Qt3DCore::QTransform();

  // add an etry, so all coordinates are local
  m_rootEntity = new Qt3DCore::QEntity( rootEntity );
  m_rootEntity->addComponent( m_rootEntityTransform );

  Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
  material->setDiffuse( QColor( QRgb( 0x668823 ) ) );

  // wheel left
  {
    m_wheelMesh = new Qt3DExtras::QCylinderMesh();
    m_wheelLeftTransform = new Qt3DCore::QTransform();

    m_wheelLeftEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelLeftEntity->addComponent( m_wheelMesh );
    m_wheelLeftEntity->addComponent( material );
    m_wheelLeftEntity->addComponent( m_wheelLeftTransform );
  }

  // wheel right
  {
    m_wheelRightTransform = new Qt3DCore::QTransform();

    m_wheelRightEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_wheelRightEntity->addComponent( m_wheelMesh );
    m_wheelRightEntity->addComponent( material );
    m_wheelRightEntity->addComponent( m_wheelRightTransform );
  }

  // hitch
  {
    m_hitchMesh = new Qt3DExtras::QCylinderMesh();
    m_hitchMesh->setRadius( 0.1 );

    m_hitchTransform = new Qt3DCore::QTransform();
    m_hitchTransform->setRotation(
      QQuaternion::fromAxisAndAngle(
        QVector3D( 0.0f, 0.0f, 1.0f ),
        90 ) );

    m_hitchEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_hitchEntity->addComponent( m_hitchMesh );
    m_hitchEntity->addComponent( material );
    m_hitchEntity->addComponent( m_hitchTransform );
  }

  // axle
  {
    m_axleMesh = new Qt3DExtras::QCylinderMesh();
    m_axleMesh->setRadius( 0.1 );

    m_axleTransform = new Qt3DCore::QTransform();

    m_axleEntity = new Qt3DCore::QEntity( m_rootEntity );
    m_axleEntity->addComponent( m_axleMesh );
    m_axleEntity->addComponent( material );
    m_axleEntity->addComponent( m_axleTransform );
  }

  // most dimensions are dependent on the wheelbase -> the code for that is in the slot setWheelbase(float)
  setProportions();

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
}

// order is important! Crashes if a parent entity is removed first!
TrailerModel::~TrailerModel() {
  delete m_hitchMesh;
  delete m_axleMesh;
  delete m_wheelMesh;

  delete m_rootEntityTransform;
  delete m_hitchTransform;
  delete m_axleTransform;
  delete m_wheelLeftTransform;
  delete m_wheelRightTransform;

  delete m_towHookMesh;
  delete m_pivotPointMesh;
  delete m_towPointMesh;

  delete m_towHookTransform;
  delete m_pivotPointTransform;
  delete m_towPointTransform;

  delete m_wheelLeftEntity;
  delete m_wheelRightEntity;

  delete m_towHookEntity;
  delete m_pivotPointEntity;
  delete m_towPointEntity;

  delete m_hitchEntity;
  delete m_axleEntity;

  delete m_rootEntity;
}

void TrailerModel::setProportions() {

  // wheels
  {
    m_wheelMesh->setRadius( m_wheelbase / 4 );
    m_wheelMesh->setLength( m_wheelbase / 5 );
    float offsetForWheels = m_wheelbase / 4 * 3 / 2 + .1;
    m_wheelLeftTransform->setTranslation( QVector3D( 0, offsetForWheels + m_wheelMesh->length() / 2, m_wheelMesh->radius() ) );
    m_wheelRightTransform->setTranslation( QVector3D( 0, -( offsetForWheels + m_wheelMesh->length() / 2 ),  m_wheelMesh->radius() ) );
  }

  // hitch
  {
    m_hitchMesh->setLength( m_offsetHookPoint.x() );
    m_hitchTransform->setTranslation( QVector3D( m_offsetHookPoint.x() / 2, 0, m_wheelMesh->radius() ) );
  }

  // axle
  {
    m_axleMesh->setLength( m_wheelbase );
    m_axleTransform->setTranslation( QVector3D( 0, 0, m_wheelMesh->radius() ) );
  }

}

void TrailerModel::setOffsetHookPointPosition( QVector3D position ) {
  m_offsetHookPoint = position;
  setProportions();
}

void TrailerModel::setPoseTowPoint( QVector3D position, QQuaternion ) {
  m_towPointTransform->setTranslation( position );
}

void TrailerModel::setPoseHookPoint( QVector3D position, QQuaternion ) {
  m_towHookTransform->setTranslation( position );
}

void TrailerModel::setWheelbase( float wheelbase ) {
  m_wheelbase = wheelbase;
  setProportions();
}

QVector3D TrailerModel::position() {
  return m_rootEntityTransform->translation();
}

QQuaternion TrailerModel::rotation() {
  return m_rootEntityTransform->rotation();
}

void TrailerModel::setPosePivotPoint( QVector3D position, QQuaternion rotation ) {
  m_pivotPointTransform->setTranslation( position );

  m_rootEntityTransform->setTranslation( position );
  m_rootEntityTransform->setRotation( rotation );
  emit positionChanged( position );
  emit rotationChanged( rotation );
}
