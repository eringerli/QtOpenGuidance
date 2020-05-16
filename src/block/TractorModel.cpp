// Copyright( C ) 2020 Christian Riggenbach
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

#include <Qt3DExtras/QMetalRoughMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>


TractorModel::TractorModel( Qt3DCore::QEntity* rootEntity, bool usePBR ) {
  qDebug() << "TractorModel::TractorModel" << rootEntity << usePBR;


  // add an etry, so all coordinates are local
  m_rootEntity = new Qt3DCore::QEntity( rootEntity );
  m_rootEntityTransform = new Qt3DCore::QTransform( m_rootEntity );
  m_rootEntity->addComponent( m_rootEntityTransform );

  const QColor colorBody = QColor( QRgb( 0x665423 ) );
  const QColor colorWheels = usePBR ? QColor( QRgb( 0x668823 ) ) : QColor( QRgb( 0x475f18 ) );

  constexpr float metalness = 0.1f;
  constexpr float roughness = 0.5f;

  // base
  {
    m_baseEntity = new Qt3DCore::QEntity( m_rootEntity );

    m_baseMesh = new Qt3DExtras::QCuboidMesh( m_baseEntity );
    m_baseTransform = new Qt3DCore::QTransform( m_baseEntity );

    if( usePBR ) {
      auto* material = new Qt3DExtras::QMetalRoughMaterial( m_baseEntity );
      material->setBaseColor( colorBody );
      material->setMetalness( metalness );
      material->setRoughness( roughness );
      m_baseEntity->addComponent( material );
    } else {
      auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_baseEntity );
      material->setDiffuse( colorBody );
      m_baseEntity->addComponent( material );
    }

    m_baseEntity->addComponent( m_baseMesh );
    m_baseEntity->addComponent( m_baseTransform );
  }

  m_wheelFrontMesh = new Qt3DExtras::QCylinderMesh( m_rootEntity );
  m_wheelFrontMesh->setRings( 5 );
  m_wheelFrontMesh->setSlices( 50 );

  // wheel front left
  {
    m_wheelFrontLeftEntity = new Qt3DCore::QEntity( m_rootEntity );

    m_wheelFrontLeftTransform = new Qt3DCore::QTransform( m_wheelFrontLeftEntity );

    if( usePBR ) {
      auto* material = new Qt3DExtras::QMetalRoughMaterial( m_wheelFrontLeftEntity );
      material->setBaseColor( colorWheels );
      material->setMetalness( metalness );
      material->setRoughness( roughness );
      m_wheelFrontLeftEntity->addComponent( material );
    } else {
      auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_wheelFrontLeftEntity );
      material->setDiffuse( colorWheels );
      m_wheelFrontLeftEntity->addComponent( material );
    }

    m_wheelFrontLeftEntity->addComponent( m_wheelFrontMesh );
    m_wheelFrontLeftEntity->addComponent( m_wheelFrontLeftTransform );
  }

  // wheel front right
  {
    m_wheelFrontRightEntity = new Qt3DCore::QEntity( m_rootEntity );

    m_wheelFrontRightTransform = new Qt3DCore::QTransform( m_wheelFrontRightEntity );

    if( usePBR ) {
      auto* material = new Qt3DExtras::QMetalRoughMaterial( m_wheelFrontRightEntity );
      material->setBaseColor( colorWheels );
      material->setMetalness( metalness );
      material->setRoughness( roughness );
      m_wheelFrontRightEntity->addComponent( material );
    } else {
      auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_wheelFrontRightEntity );
      material->setDiffuse( colorWheels );
      m_wheelFrontRightEntity->addComponent( material );
    }

    m_wheelFrontRightEntity->addComponent( m_wheelFrontMesh );
    m_wheelFrontRightEntity->addComponent( m_wheelFrontRightTransform );
  }

  m_wheelBackMesh = new Qt3DExtras::QCylinderMesh( m_rootEntity );
  m_wheelBackMesh->setRings( 5 );
  m_wheelBackMesh->setSlices( 50 );

  // wheel back left
  {
    m_wheelBackLeftEntity = new Qt3DCore::QEntity( m_rootEntity );

    m_wheelBackLeftTransform = new Qt3DCore::QTransform( m_wheelBackLeftEntity );

    if( usePBR ) {
      auto* material = new Qt3DExtras::QMetalRoughMaterial( m_wheelBackLeftEntity );
      material->setBaseColor( colorWheels );
      material->setMetalness( metalness );
      material->setRoughness( roughness );
      m_wheelBackLeftEntity->addComponent( material );
    } else {
      auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_wheelBackLeftEntity );
      material->setDiffuse( colorWheels );
      m_wheelBackLeftEntity->addComponent( material );
    }

    m_wheelBackLeftEntity->addComponent( m_wheelBackMesh );
    m_wheelBackLeftEntity->addComponent( m_wheelBackLeftTransform );
  }

  // wheel back right
  {
    m_wheelBackRightEntity = new Qt3DCore::QEntity( m_rootEntity );

    m_wheelBackRightTransform = new Qt3DCore::QTransform( m_wheelBackRightEntity );

    if( usePBR ) {
      auto* material = new Qt3DExtras::QMetalRoughMaterial( m_wheelBackRightEntity );
      material->setBaseColor( colorWheels );
      material->setMetalness( metalness );
      material->setRoughness( roughness );
      m_wheelBackRightEntity->addComponent( material );
    } else {
      auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_wheelBackRightEntity );
      material->setDiffuse( colorWheels );
      m_wheelBackRightEntity->addComponent( material );
    }

    m_wheelBackRightEntity->addComponent( m_wheelBackMesh );
    m_wheelBackRightEntity->addComponent( m_wheelBackRightTransform );
  }

  // most dimensions are dependent on the wheelbase -> the code for that is in the slot setWheelbase(float)
  setWheelbase( m_wheelbase );

  // everything in world-coordinates... (add to rootEntity, not m_rootEntity)
  {
    // tow hook marker -> red
    {
      m_towHookEntity = new Qt3DCore::QEntity( rootEntity );

      m_towHookMesh = new Qt3DExtras::QSphereMesh( m_towHookEntity );
      m_towHookMesh->setRadius( .2f );
      m_towHookMesh->setSlices( 20 );
      m_towHookMesh->setRings( 20 );

      m_towHookTransform = new Qt3DCore::QTransform( m_towHookEntity );

      if( usePBR ) {
        auto* material = new Qt3DExtras::QMetalRoughMaterial( m_towHookEntity );
        material->setBaseColor( QColor( Qt::darkRed ) );
        material->setMetalness( metalness );
        material->setRoughness( roughness );
        m_towHookEntity->addComponent( material );
      } else {
        auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_towHookEntity );
        material->setDiffuse( QColor( QRgb( 0xaa3333 ) ) );
        m_towHookEntity->addComponent( material );
      }

      m_towHookEntity->addComponent( m_towHookMesh );
      m_towHookEntity->addComponent( m_towHookTransform );
    }

    // pivot point marker -> green
    {
      m_pivotPointEntity = new Qt3DCore::QEntity( rootEntity );

      m_pivotPointMesh = new Qt3DExtras::QSphereMesh( m_pivotPointEntity );
      m_pivotPointMesh->setRadius( .2f );
      m_pivotPointMesh->setSlices( 20 );
      m_pivotPointMesh->setRings( 20 );

      m_pivotPointTransform = new Qt3DCore::QTransform( m_pivotPointEntity );

      if( usePBR ) {
        auto* material = new Qt3DExtras::QMetalRoughMaterial( m_pivotPointEntity );
        material->setBaseColor( QColor( Qt::darkGreen ) );
        material->setMetalness( metalness );
        material->setRoughness( roughness );
        m_pivotPointEntity->addComponent( material );
      } else {
        auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_pivotPointEntity );
        material->setDiffuse( QColor( QRgb( 0x33aa33 ) ) );
        m_pivotPointEntity->addComponent( material );
      }

      m_pivotPointEntity->addComponent( m_pivotPointMesh );
      m_pivotPointEntity->addComponent( m_pivotPointTransform );
    }

    // hitch marker -> blue
    {
      m_towPointEntity = new Qt3DCore::QEntity( rootEntity );

      m_towPointMesh = new Qt3DExtras::QSphereMesh( m_towPointEntity );
      m_towPointMesh->setRadius( .2f );
      m_towPointMesh->setSlices( 20 );
      m_towPointMesh->setRings( 20 );

      m_towPointTransform = new Qt3DCore::QTransform( m_towPointEntity );

      if( usePBR ) {
        auto* material = new Qt3DExtras::QMetalRoughMaterial( m_towPointEntity );
        material->setBaseColor( QColor( Qt::darkBlue ) );
        material->setMetalness( metalness );
        material->setRoughness( roughness );
        m_towPointEntity->addComponent( material );
      } else {
        auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_towPointEntity );
        material->setDiffuse( QColor( QRgb( 0x3333aa ) ) );
        m_towPointEntity->addComponent( material );
      }

      m_towPointEntity->addComponent( m_towPointMesh );
      m_towPointEntity->addComponent( m_towPointTransform );
    }
  }
}

// order is important! Crashes if a parent entity is removed first!
TractorModel::~TractorModel() {
  m_towHookEntity->setEnabled( false );
  m_pivotPointEntity->setEnabled( false );
  m_towPointEntity->setEnabled( false );
  m_rootEntity->setEnabled( false );

  m_towHookEntity->deleteLater();
  m_pivotPointEntity->deleteLater();
  m_towPointEntity->deleteLater();
  m_rootEntity->deleteLater();
}

void TractorModel::setWheelbase( double wheelbase ) {
  if( !qFuzzyIsNull( wheelbase ) ) {
    m_wheelbase = wheelbase;
    setProportions();
  }
}

void TractorModel::setTrackwidth( double trackwidth ) {
  if( !qFuzzyIsNull( trackwidth ) ) {
    m_trackwidth = trackwidth;
    setProportions();
  }
}

void TractorModel::setProportions() {
  float offsetForWheels = m_trackwidth / 2;

  // base
  {
    m_baseMesh->setYExtent( m_trackwidth - 0.2f );
    m_baseMesh->setXExtent( m_wheelbase );
    m_baseMesh->setZExtent( m_wheelbase / 4 );

    m_baseTransform->setTranslation( QVector3D( m_baseMesh->xExtent() / 2, 0, m_baseMesh->zExtent() / 2 + 0.25f ) );
  }

  // wheels front
  {
    m_wheelFrontMesh->setRadius( m_wheelbase / 4 );
    m_wheelFrontMesh->setLength( m_wheelbase / 5 );
    m_wheelFrontLeftTransform->setTranslation( QVector3D( m_wheelbase, offsetForWheels + m_wheelFrontMesh->length() / 2, m_wheelFrontMesh->radius() ) );
    m_wheelFrontRightTransform->setTranslation( QVector3D( m_wheelbase, -( offsetForWheels + m_wheelFrontMesh->length() / 2 ),  m_wheelFrontMesh->radius() ) );
  }

  // wheels back
  {
    m_wheelBackMesh->setRadius( m_wheelbase / 2 );
    m_wheelBackMesh->setLength( m_wheelbase / 5 );
    m_wheelBackLeftTransform->setTranslation( QVector3D( 0, offsetForWheels + m_wheelBackMesh->length() / 2,  m_wheelBackMesh->radius() ) );
    m_wheelBackRightTransform->setTranslation( QVector3D( 0, -( offsetForWheels + m_wheelBackMesh->length() / 2 ),  m_wheelBackMesh->radius() ) );
  }
}

void TractorModel::setPoseTowPoint( const Point_3& position, const QQuaternion, const PoseOption::Options options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    m_towPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );
  }
}

void TractorModel::setPoseHookPoint( const Point_3& position, const QQuaternion, const PoseOption::Options options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    m_towHookTransform->setTranslation( convertPoint3ToQVector3D( position ) );
  }
}

void TractorModel::setPosePivotPoint( const Point_3& position, const QQuaternion rotation, const PoseOption::Options options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    m_pivotPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );

    m_rootEntityTransform->setTranslation( convertPoint3ToQVector3D( position ) );
    m_rootEntityTransform->setRotation( rotation );
  }
}

void TractorModel::setSteeringAngleLeft( double steerAngle ) {
  QQuaternion rotationLeft =  QQuaternion::fromAxisAndAngle( QVector3D( 0.0F, 0.0F, 1.0F ), steerAngle );
  m_wheelFrontLeftTransform->setRotation( rotationLeft );
}
void TractorModel::setSteeringAngleRight( double steerAngle ) {
  QQuaternion rotationRight =  QQuaternion::fromAxisAndAngle( QVector3D( 0.0F, 0.0F, 1.0F ), steerAngle );
  m_wheelFrontRightTransform->setRotation( rotationRight );
}
