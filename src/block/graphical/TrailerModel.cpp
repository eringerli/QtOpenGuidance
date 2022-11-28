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

#include "TrailerModel.h"

#include <QAction>
#include <QBrush>
#include <QMenu>

#include "qneblock.h"
#include "qneport.h"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DCore/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QMetalRoughMaterial>

TrailerModel::TrailerModel( Qt3DCore::QEntity* rootEntity, bool usePBR ) {
  // add an etry, so all coordinates are local
  m_rootEntity          = new Qt3DCore::QEntity( rootEntity );
  m_rootEntityTransform = new Qt3DCore::QTransform( m_rootEntity );
  m_rootEntity->addComponent( m_rootEntityTransform );

  const QColor    colorBody   = QColor( QRgb( 0x665423 ) );
  const QColor    colorWheels = usePBR ? QColor( QRgb( 0x668823 ) ) : QColor( QRgb( 0x475f18 ) );
  constexpr float metalness   = 0.1f;
  constexpr float roughness   = 0.5f;

  // wheel left
  {
    m_wheelLeftEntity = new Qt3DCore::QEntity( m_rootEntity );

    m_wheelMesh = new Qt3DExtras::QCylinderMesh( m_rootEntity );
    m_wheelMesh->setRings( 5 );
    m_wheelMesh->setSlices( 50 );
    m_wheelLeftTransform = new Qt3DCore::QTransform( m_wheelLeftEntity );

    if( usePBR ) {
      auto* material = new Qt3DExtras::QMetalRoughMaterial( m_wheelLeftEntity );
      material->setBaseColor( colorWheels );
      material->setMetalness( metalness );
      material->setRoughness( roughness );
      m_wheelLeftEntity->addComponent( material );
    } else {
      auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_wheelLeftEntity );
      material->setDiffuse( colorWheels );
      m_wheelLeftEntity->addComponent( material );
    }

    m_wheelLeftEntity->addComponent( m_wheelMesh );
    m_wheelLeftEntity->addComponent( m_wheelLeftTransform );
  }

  // wheel right
  {
    m_wheelRightEntity = new Qt3DCore::QEntity( m_rootEntity );

    m_wheelRightTransform = new Qt3DCore::QTransform( m_wheelRightEntity );

    if( usePBR ) {
      auto* material = new Qt3DExtras::QMetalRoughMaterial( m_wheelRightEntity );
      material->setBaseColor( colorWheels );
      material->setMetalness( metalness );
      material->setRoughness( roughness );
      m_wheelRightEntity->addComponent( material );
    } else {
      auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_wheelRightEntity );
      material->setDiffuse( colorWheels );
      m_wheelRightEntity->addComponent( material );
    }

    m_wheelRightEntity->addComponent( m_wheelMesh );
    m_wheelRightEntity->addComponent( m_wheelRightTransform );
  }

  // hitch
  {
    m_hitchEntity = new Qt3DCore::QEntity( m_rootEntity );

    m_hitchMesh = new Qt3DExtras::QCylinderMesh( m_hitchEntity );
    m_hitchMesh->setRadius( 0.1f );

    m_hitchTransform = new Qt3DCore::QTransform( m_hitchEntity );
    m_hitchTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0.0f, 0.0f, 1.0f ), 90 ) );

    if( usePBR ) {
      auto* material = new Qt3DExtras::QMetalRoughMaterial( m_hitchEntity );
      material->setBaseColor( colorBody );
      material->setMetalness( metalness );
      material->setRoughness( roughness );
      m_hitchEntity->addComponent( material );
    } else {
      auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_hitchEntity );
      material->setDiffuse( colorBody );
      m_hitchEntity->addComponent( material );
    }

    m_hitchEntity->addComponent( m_hitchMesh );
    m_hitchEntity->addComponent( m_hitchTransform );
  }

  // axle
  {
    m_axleEntity = new Qt3DCore::QEntity( m_rootEntity );

    m_axleMesh = new Qt3DExtras::QCylinderMesh( m_axleEntity );
    m_axleMesh->setRadius( 0.1f );

    m_axleTransform = new Qt3DCore::QTransform( m_axleEntity );

    if( usePBR ) {
      auto* material = new Qt3DExtras::QMetalRoughMaterial( m_axleEntity );
      material->setBaseColor( colorBody );
      material->setMetalness( metalness );
      material->setRoughness( roughness );
      m_axleEntity->addComponent( material );
    } else {
      auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( m_axleEntity );
      material->setDiffuse( colorBody );
      m_axleEntity->addComponent( material );
    }

    m_axleEntity->addComponent( m_axleMesh );
    m_axleEntity->addComponent( m_axleTransform );
  }

  // most dimensions are dependent on the wheelbase -> the code for that is in the slot
  // setWheelbase(float)
  setProportions();

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
TrailerModel::~TrailerModel() {
  m_towHookEntity->setEnabled( false );
  m_pivotPointEntity->setEnabled( false );
  m_towPointEntity->setEnabled( false );
  m_rootEntity->setEnabled( false );

  m_towHookEntity->deleteLater();
  m_pivotPointEntity->deleteLater();
  m_towPointEntity->deleteLater();
  m_rootEntity->deleteLater();
}

void
TrailerModel::setProportions() {
  // wheels
  {
    m_wheelMesh->setRadius( m_trackwidth / 4 );
    m_wheelMesh->setLength( m_trackwidth / 5 );

    float offsetForWheels = m_trackwidth / 2;

    m_wheelLeftTransform->setTranslation( QVector3D( 0, offsetForWheels + m_wheelMesh->length() / 2, m_wheelMesh->radius() ) );
    m_wheelRightTransform->setTranslation( QVector3D( 0, -( offsetForWheels + m_wheelMesh->length() / 2 ), m_wheelMesh->radius() ) );
  }

  // hitch
  {
    m_hitchMesh->setLength( m_offsetHookPoint.x() );
    m_hitchTransform->setTranslation( QVector3D( m_offsetHookPoint.x() / 2, 0, m_wheelMesh->radius() ) );
  }

  // axle
  {
    m_axleMesh->setLength( m_trackwidth );
    m_axleTransform->setTranslation( QVector3D( 0, 0, m_wheelMesh->radius() ) );
  }
}

void
TrailerModel::setOffsetHookPointPosition( const Eigen::Vector3d& position ) {
  m_offsetHookPoint = position;
  setProportions();
}

void
TrailerModel::setTrackwidth( double trackwidth, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) ) {
    if( !qFuzzyIsNull( trackwidth ) ) {
      m_trackwidth = trackwidth;
      setProportions();
    }
  }
}

void
TrailerModel::setPoseTowPoint( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) ) {
    m_towPointTransform->setTranslation( toQVector3D( position ) );
  }
}

void
TrailerModel::setPoseHookPoint( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) ) {
    m_towHookTransform->setTranslation( toQVector3D( position ) );
  }
}

void
TrailerModel::setPosePivotPoint( const Eigen::Vector3d&           position,
                                 const Eigen::Quaterniond&        orientation,
                                 const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) ) {
    m_pivotPointTransform->setTranslation( toQVector3D( position ) );

    m_rootEntityTransform->setTranslation( toQVector3D( position ) );
    m_rootEntityTransform->setRotation( toQQuaternion( orientation ) );
  }
}

QNEBlock*
TrailerModelFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new TrailerModel( rootEntity, usePBR );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Track Width" ), QLatin1String( SLOT( setTrackwidth( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Offset Hook Point" ), QLatin1String( SLOT( setOffsetHookPointPosition( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SLOT( setPoseHookPoint( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SLOT( setPosePivotPoint( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SLOT( setPoseTowPoint( POSE_SIGNATURE ) ) ) );

  b->setBrush( modelColor );

  return b;
}
