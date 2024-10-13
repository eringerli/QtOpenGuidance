// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SprayerModel.h"

#include <QColor>
#include <QPointer>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DCore/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DExtras/QCylinderMesh>

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QMetalRoughMaterial>

#include <Qt3DExtras/QConeMesh>

#include "3d/BufferMesh.h"

#include "block/sectionControl/Implement.h"
#include "block/sectionControl/ImplementSection.h"

#include "helpers/eigenHelper.h"

SprayerModel::SprayerModel( Qt3DCore::QEntity* rootEntity, bool usePBR, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ) {
  // add an entry, so all coordinates are local
  m_rootEntity          = new Qt3DCore::QEntity( rootEntity );
  m_rootEntityTransform = new Qt3DCore::QTransform( m_rootEntity );
  m_rootEntity->addComponent( m_rootEntityTransform );

  this->usePBR = usePBR;
}

SprayerModel::~SprayerModel() {
  m_rootEntity->setEnabled( false );
  m_rootEntity->deleteLater();
}

void
SprayerModel::enable( const bool enable ) {
  BlockBase::enable( enable );

  m_rootEntity->setEnabled( enable );
}

void
SprayerModel::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::Graphical ) ) {
    m_rootEntityTransform->setTranslation( toQVector3D( position ) );
    m_rootEntityTransform->setRotation( toQQuaternion( orientation ) );
  }
}

void
SprayerModel::setSections() {
  if( implement != nullptr ) {
    int numSections = implement->sections.size();
    --numSections;

    const auto& section0       = implement->sections.at( 0 );
    const auto& state0         = section0->state();
    const bool  globalForceOff = state0.testFlag( ImplementSection::State::ForceOff );
    const bool  globalForceOn  = state0.testFlag( ImplementSection::State::ForceOn );

    for( int sectionIndex = 0; sectionIndex < numSections; ++sectionIndex ) {
      const auto& section = implement->sections.at( sectionIndex + 1 );
      const auto& state   = section->state();

      if( state.testFlag( ImplementSection::State::ForceOff ) || globalForceOff ) {
        forcedOffBoomEntities.at( sectionIndex )->setEnabled( true );
        forcedOnBoomEntities.at( sectionIndex )->setEnabled( false );
        onBoomEntities.at( sectionIndex )->setEnabled( false );
        offBoomEntities.at( sectionIndex )->setEnabled( false );
        sprayEntities.at( sectionIndex )->setEnabled( false );
      } else {
        if( state.testFlag( ImplementSection::State::ForceOn ) || globalForceOn ) {
          forcedOffBoomEntities.at( sectionIndex )->setEnabled( false );
          forcedOnBoomEntities.at( sectionIndex )->setEnabled( true );
          onBoomEntities.at( sectionIndex )->setEnabled( false );
          offBoomEntities.at( sectionIndex )->setEnabled( false );
          sprayEntities.at( sectionIndex )->setEnabled( true );
        } else {
          if( section->isSectionOn() ) {
            forcedOffBoomEntities.at( sectionIndex )->setEnabled( false );
            forcedOnBoomEntities.at( sectionIndex )->setEnabled( false );
            onBoomEntities.at( sectionIndex )->setEnabled( true );
            offBoomEntities.at( sectionIndex )->setEnabled( false );
            sprayEntities.at( sectionIndex )->setEnabled( true );
          } else {
            forcedOffBoomEntities.at( sectionIndex )->setEnabled( false );
            forcedOnBoomEntities.at( sectionIndex )->setEnabled( false );
            onBoomEntities.at( sectionIndex )->setEnabled( false );
            offBoomEntities.at( sectionIndex )->setEnabled( true );
            sprayEntities.at( sectionIndex )->setEnabled( false );
          }
        }
      }
    }
  }
}

void
SprayerModel::setImplement( const QPointer< Implement >& implement ) {
  if( implement != nullptr ) {
    this->implement = implement;

    size_t numSections = implement->sections.size();
    --numSections;

    if( numSections != boomEntities.size() && numSections > 0 ) {
      // delete old sections
      for( const auto& entity : boomEntities ) {
        entity->deleteLater();
      }

      boomEntities.clear();
      boomMeshes.clear();
      boomTransforms.clear();
      forcedOnBoomEntities.clear();
      forcedOffBoomEntities.clear();
      onBoomEntities.clear();
      offBoomEntities.clear();
      sprayEntities.clear();
      sprayTransforms.clear();

      boomEntities.reserve( numSections );
      boomMeshes.reserve( numSections );
      boomTransforms.reserve( numSections );
      forcedOnBoomEntities.reserve( numSections );
      forcedOffBoomEntities.reserve( numSections );
      onBoomEntities.reserve( numSections );
      offBoomEntities.reserve( numSections );
      sprayEntities.reserve( numSections );
      sprayTransforms.reserve( numSections );

      // create new sections
      for( size_t i = 0; i < numSections; ++i ) {
        auto* entity = new Qt3DCore::QEntity( m_rootEntity );
        boomEntities.push_back( entity );

        auto* boomRootEntity = new Qt3DCore::QEntity( entity );

        auto* boomMeshTransform = new Qt3DCore::QTransform( boomRootEntity );
        boomRootEntity->addComponent( boomMeshTransform );
        boomTransforms.push_back( boomMeshTransform );

        auto* boomMesh = new Qt3DExtras::QCylinderMesh( boomRootEntity );
        boomMesh->setRadius( 0.3f );
        boomMesh->setRings( 20.0f );
        boomMesh->setSlices( 20.0f );
        boomMeshes.push_back( boomMesh );

        const QColor colorForceOn  = QColor( Qt::green );
        const QColor colorForceOff = QColor( Qt::red );
        const QColor colorOn       = QColor( Qt::darkGreen );
        const QColor colorOff      = QColor( Qt::darkRed );

        constexpr float boomMetalness = 0.1f;
        constexpr float boomRoughness = 0.5f;

        {
          auto* boomEntity = new Qt3DCore::QEntity( boomRootEntity );

          if( usePBR ) {
            auto* materialBoom = new Qt3DExtras::QMetalRoughMaterial( boomEntity );
            materialBoom->setBaseColor( colorForceOff );
            materialBoom->setMetalness( boomMetalness );
            materialBoom->setRoughness( boomRoughness );
            boomEntity->addComponent( materialBoom );
          } else {
            auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( boomEntity );
            material->setDiffuse( colorForceOff );
            boomEntity->addComponent( material );
          }

          auto* boomTransform = new Qt3DCore::QTransform( boomRootEntity );

          boomEntity->addComponent( boomMesh );
          boomEntity->addComponent( boomTransform );

          forcedOffBoomEntities.push_back( boomEntity );
        }
        {
          auto* boomEntity = new Qt3DCore::QEntity( boomRootEntity );

          if( usePBR ) {
            auto* materialBoom = new Qt3DExtras::QMetalRoughMaterial( boomEntity );
            materialBoom->setBaseColor( colorForceOn );
            materialBoom->setMetalness( boomMetalness );
            materialBoom->setRoughness( boomRoughness );
            boomEntity->addComponent( materialBoom );
          } else {
            auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( boomEntity );
            material->setDiffuse( colorForceOn );
            boomEntity->addComponent( material );
          }

          auto* boomTransform = new Qt3DCore::QTransform( boomRootEntity );

          boomEntity->addComponent( boomMesh );
          boomEntity->addComponent( boomTransform );

          forcedOnBoomEntities.push_back( boomEntity );
        }
        {
          auto* boomEntity = new Qt3DCore::QEntity( boomRootEntity );

          if( usePBR ) {
            auto* materialBoom = new Qt3DExtras::QMetalRoughMaterial( boomEntity );
            materialBoom->setBaseColor( colorOn );
            materialBoom->setMetalness( boomMetalness );
            materialBoom->setRoughness( boomRoughness );
            boomEntity->addComponent( materialBoom );
          } else {
            auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( boomEntity );
            material->setDiffuse( colorOn );
            boomEntity->addComponent( material );
          }

          auto* boomTransform = new Qt3DCore::QTransform( boomRootEntity );

          boomEntity->addComponent( boomMesh );
          boomEntity->addComponent( boomTransform );

          onBoomEntities.push_back( boomEntity );
        }
        {
          auto* boomEntity = new Qt3DCore::QEntity( boomRootEntity );

          if( usePBR ) {
            auto* materialBoom = new Qt3DExtras::QMetalRoughMaterial( boomEntity );
            materialBoom->setBaseColor( colorOff );
            materialBoom->setMetalness( boomMetalness );
            materialBoom->setRoughness( boomRoughness );
            boomEntity->addComponent( materialBoom );
          } else {
            auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( boomEntity );
            material->setDiffuse( colorOff );
            boomEntity->addComponent( material );
          }

          auto* boomTransform = new Qt3DCore::QTransform( boomRootEntity );

          boomEntity->addComponent( boomMesh );
          boomEntity->addComponent( boomTransform );

          offBoomEntities.push_back( boomEntity );
        }

        {
          auto* sprayEntity = new Qt3DCore::QEntity( boomRootEntity );

          auto* sprayMesh = new Qt3DExtras::QConeMesh( sprayEntity );
          sprayMesh->setRings( 20 );
          sprayMesh->setSlices( 20 );
          sprayMesh->setTopRadius( 0 );
          sprayMesh->setBottomRadius( 0.5f );
          sprayMesh->setLength( 1 );
          sprayMesh->setHasTopEndcap( false );
          sprayMesh->setHasBottomEndcap( false );

          auto* sprayTransform = new Qt3DCore::QTransform( sprayEntity );
          sprayTransform->setRotationX( 90 );

          if( usePBR ) {
            auto* sprayMaterial = new Qt3DExtras::QMetalRoughMaterial( sprayEntity );
            sprayMaterial->setBaseColor( sprayColor );
            sprayMaterial->setMetalness( 0.0f );
            sprayMaterial->setRoughness( 0.5f );
            sprayEntity->addComponent( sprayMaterial );
          } else {
            auto* sprayMaterial = new Qt3DExtras::QDiffuseSpecularMaterial( sprayEntity );
            sprayMaterial->setDiffuse( sprayColor );
            sprayEntity->addComponent( sprayMaterial );
          }

          sprayEntity->addComponent( sprayMesh );
          sprayEntity->addComponent( sprayTransform );

          sprayEntities.push_back( sprayEntity );
          sprayTransforms.push_back( sprayTransform );
        }
      }
    }

    updateProprotions();

    setSections();
  }
}

void
SprayerModel::setHeight( double height, const CalculationOption::Options ) {
  this->m_height = height;
  updateProprotions();
}

void
SprayerModel::updateProprotions() {
  if( implement != nullptr ) {
    int numSections = implement->sections.size();
    --numSections;

    // get the left most point of the implement
    double sectionOffset = 0;

    for( const auto& section : implement->sections ) {
      sectionOffset += section->widthOfSection - section->overlapLeft - section->overlapRight;
    }

    sectionOffset = sectionOffset / 2;

    for( int i = 0; i < numSections; ++i ) {
      auto* const section = implement->sections.at( i + 1 );
      sectionOffset += section->overlapLeft - section->widthOfSection;

      boomMeshes.at( i )->setLength( float( section->widthOfSection ) );
      boomTransforms.at( i )->setTranslation( QVector3D( 0, float( sectionOffset ) + ( section->widthOfSection / 2 ), m_height ) );

      sprayTransforms.at( i )->setScale3D( QVector3D( m_height / 3, m_height, section->widthOfSection ) );
      sprayTransforms.at( i )->setTranslation( QVector3D( 0, 0, -m_height / 2 ) );

      sectionOffset += section->overlapRight;
    }
  }
}

std::unique_ptr< BlockBase >
SprayerModelFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< SprayerModel >( idHint, rootEntity, usePBR );

  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Height" ), obj.get(), QLatin1StringView( SLOT( setHeight( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Implement Data" ), obj.get(), QLatin1StringView( SLOT( setImplement( const QPointer< Implement > ) ) ) );
  obj->addInputPort( QStringLiteral( "Section Control Data" ), obj.get(), QLatin1StringView( SLOT( setSections() ) ) );

  return obj;
}
