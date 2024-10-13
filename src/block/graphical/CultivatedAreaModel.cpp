// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CultivatedAreaModel.h"

#include <QtCore/QDebug>
#include <QtMath>

#include "3d/CultivatedAreaMesh.h"

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QMetalRoughMaterial>

#include "block/sectionControl/Implement.h"
#include "block/sectionControl/ImplementSection.h"

#include "block/kinematic/FixedKinematicPrimitive.h"
#include "kinematic/CgalWorker.h"
#include "kinematic/cgal.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

#include "gui/toolbar/NewOpenSaveToolbar.h"

#include <QAction>
#include <QFile>
#include <QMenu>
#include <QPointer>

void
CultivatedAreaModel::createEntities() {
  const QColor colorCultivatedArea = QColor( 0xa2, 0xb8, 0xff, 128 );

  // base
  m_baseEntity    = new Qt3DCore::QEntity( m_rootEntity );
  m_baseTransform = new Qt3DCore::QTransform( m_baseEntity );
  m_baseEntity->addComponent( m_baseTransform );

  m_phongMaterial = new Qt3DExtras::QDiffuseSpecularMaterial( m_baseEntity );
  m_phongMaterial->setDiffuse( colorCultivatedArea );
  m_phongMaterial->setAmbient( colorCultivatedArea );
  m_phongMaterial->setSpecular( colorCultivatedArea );
  m_phongMaterial->setAlphaBlendingEnabled( true );
  m_phongMaterial->setShininess( 0 );

  m_layer = new Qt3DRender::QLayer( m_baseEntity );
  m_layer->setRecursive( true );
  m_baseEntity->addComponent( m_layer );
}

CultivatedAreaModel::CultivatedAreaModel(
  Qt3DCore::QEntity* rootEntity, NewOpenSaveToolbar* newOpenSaveToolbar, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ), m_rootEntity( rootEntity ) {
  // plug into new/open/save toolbar
  newCultivatedAreaAction = newOpenSaveToolbar->newMenu->addAction( QStringLiteral( "New Cultivated Area" ) );
  QObject::connect( newCultivatedAreaAction, &QAction::triggered, this, &CultivatedAreaModel::newCultivatedArea );
  openCultivatedAreaAction = newOpenSaveToolbar->openMenu->addAction( QStringLiteral( "Open Cultivated Area" ) );
  QObject::connect( openCultivatedAreaAction, &QAction::triggered, this, &CultivatedAreaModel::openCultivatedArea );
  saveCultivatedAreaAction = newOpenSaveToolbar->saveMenu->addAction( QStringLiteral( "Save Cultivated Area" ) );
  QObject::connect( saveCultivatedAreaAction, &QAction::triggered, this, &CultivatedAreaModel::saveCultivatedArea );

  createEntities();
}

// order is important! Crashes if a parent entity is removed first!
CultivatedAreaModel::~CultivatedAreaModel() {
  m_baseEntity->setEnabled( false );

  m_baseEntity->deleteLater();
}

void
CultivatedAreaModel::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT layerChanged( m_layer );
}

void
CultivatedAreaModel::setPose( const Eigen::Vector3d&           position,
                              const Eigen::Quaterniond&        orientation,
                              const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::Graphical ) ) {
    if( implement != nullptr ) {
      auto normalOfQuaternion = toQVector3D( getNormalFromQuaternion( orientation ) );

      for( size_t i = 0, end = sectionMeshes.size(); i < end; ++i ) {
        auto* const mesh = sectionMeshes.at( i );

        if( sectionStates.at( i ) ) {
          auto positionCorrectedLeft =
            toPoint3( FixedKinematicPrimitive::calculate( position, Eigen::Vector3d( 0, sectionOffsets.at( i * 2 ), 0 ), orientation ) );
          auto positionCorrectedRight = toPoint3(
            FixedKinematicPrimitive::calculate( position, Eigen::Vector3d( 0, sectionOffsets.at( ( i * 2 ) + 1 ), 0 ), orientation ) );

          mesh->addPoints( positionCorrectedLeft, positionCorrectedRight, normalOfQuaternion );

          if( mesh->numPoints() > 1000 ) {
            mesh->optimise();
            sectionMeshes.at( i ) = createNewMesh();

            sectionMeshes.at( i )->addPoints( positionCorrectedLeft, positionCorrectedRight, normalOfQuaternion );
          }
        }
      }
    }
  }
}

void
CultivatedAreaModel::setImplement( const QPointer< Implement >& implement ) {
  if( implement != nullptr ) {
    this->implement    = implement;
    size_t numSections = implement->sections.size();

    for( auto& mesh : sectionMeshes ) {
      if( mesh != nullptr ) {
        if( mesh->numPoints() > 3 ) {
          mesh->optimise();
        } else {
          mesh->setEnabled( false );
          mesh->deleteLater();
        }
      }
    }

    sectionMeshes.clear();
    sectionMeshes.resize( numSections - 1, nullptr );

    // get the left most point of the implement
    double sectionOffset = 0;

    for( size_t i = 0; i < numSections; ++i ) {
      auto* const section = implement->sections.at( i );
      sectionOffset += section->widthOfSection - section->overlapLeft - section->overlapRight;
    }

    sectionOffset /= 2;

    sectionOffsets.clear();
    sectionOffsets.reserve( ( numSections - 1 ) * 2 );

    sectionStates.clear();
    sectionStates.resize( numSections - 1, false );

    for( size_t i = 1; i < numSections; ++i ) {
      auto* const section = implement->sections.at( i );
      sectionOffset += section->overlapLeft;
      sectionOffsets.push_back( sectionOffset );
      sectionOffset -= section->widthOfSection;
      sectionOffsets.push_back( sectionOffset );
      sectionOffset += section->overlapRight;
    }
  }
}

CultivatedAreaMesh*
CultivatedAreaModel::createNewMesh() {
  auto* entity = new Qt3DCore::QEntity( m_baseEntity );

  entity->addComponent( m_phongMaterial );

  auto* mesh = new CultivatedAreaMesh( entity );
  entity->addComponent( mesh );

  return mesh;
}

void
CultivatedAreaModel::setSections() {
  if( implement != nullptr ) {
    size_t numSections = implement->sections.size() - 1;

    for( size_t i = 0; i < numSections; ++i ) {
      const auto& section = implement->sections.at( i );
      const auto& state   = section->state();

      if( sectionMeshes.at( i ) == nullptr ) {
        sectionMeshes.at( i ) = createNewMesh();
      }

      if( ( sectionStates.at( i ) = implement->isSectionOn( i ) ) ) {
      } else {
        if( sectionMeshes.at( i )->numPoints() > 3 ) {
          sectionMeshes.at( i )->optimise();
          sectionMeshes.at( i ) = createNewMesh();
        } else {
          sectionMeshes.at( i )->clear();
        }
      }
    }
  }
}

void
CultivatedAreaModel::newCultivatedArea() {
  qDebug() << "CultivatedAreaModel::newCultivatedArea()";

  m_baseEntity->setEnabled( false );
  m_layer->setEnabled( false );

  for( auto& child : m_baseEntity->children() ) {
    auto* entity = qobject_cast< Qt3DCore::QEntity* >( child );
    if( entity != nullptr ) {
      entity->setEnabled( false );
    }
  }

  m_baseEntity->deleteLater();

  createEntities();
  Q_EMIT layerChanged( m_layer );

  setImplement( implement );
  setSections();

  m_baseEntity->setEnabled( true );
}

void
CultivatedAreaModel::openCultivatedArea() {
  qDebug() << "CultivatedAreaModel::openCultivatedArea()";
}

void
CultivatedAreaModel::openCultivatedAreaFromFile( QFile& file ) {
  qDebug() << "CultivatedAreaModel::openCultivatedAreaFromFile()";
}

void
CultivatedAreaModel::saveCultivatedArea() {
  qDebug() << "CultivatedAreaModel::saveCultivatedArea()";
}

void
CultivatedAreaModel::saveCultivatedAreaToFile( QFile& file ) {
  qDebug() << "CultivatedAreaModel::saveCultivatedAreaToFile()";
}

void
CultivatedAreaModel::enable( const bool enable ) {
  BlockBase::enable( enable );

  m_baseEntity->setEnabled( enable );
}

std::unique_ptr< BlockBase >
CultivatedAreaModelFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< CultivatedAreaModel >( idHint, rootEntity, newOpenSaveToolbar );

  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Implement Data" ), obj.get(), QLatin1StringView( SLOT( setImplement( const QPointer< Implement > ) ) ) );
  obj->addInputPort( QStringLiteral( "Section Control Data" ), obj.get(), QLatin1StringView( SLOT( setSections() ) ) );

  obj->addOutputPort( QStringLiteral( "Cultivated Area" ), obj.get(), QLatin1StringView( SIGNAL( layerChanged( Qt3DRender::QLayer* ) ) ) );

  return obj;
}

#include "moc_CultivatedAreaModel.cpp"
