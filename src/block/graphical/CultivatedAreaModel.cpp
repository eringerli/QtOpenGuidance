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

#include "kinematic/cgal.h"
#include "kinematic/CgalWorker.h"
#include "CultivatedAreaModel.h"

#include <QtCore/QDebug>
#include <QtMath>

#include "3d/CultivatedAreaMesh.h"

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QMetalRoughMaterial>

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

CultivatedAreaModel::CultivatedAreaModel( Qt3DCore::QEntity* rootEntity, CgalThread* threadForCgalWorker )
  : threadForCgalWorker( threadForCgalWorker ) {
  const QColor colorCultivatedArea = QColor( 0xa2, 0xb8, 0xff, 128 );

  // base
  m_baseEntity = new Qt3DCore::QEntity( rootEntity );
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

// order is important! Crashes if a parent entity is removed first!
CultivatedAreaModel::~CultivatedAreaModel() {
  m_baseEntity->setEnabled( false );

  m_baseEntity->deleteLater();
}

void CultivatedAreaModel::emitConfigSignals() {
  Q_EMIT layerChanged( m_layer );
}

void CultivatedAreaModel::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    if( implement != nullptr ) {
      m_baseTransform->setTranslation( QVector3D( 0, 0, position.z() ) );

      for( size_t i = 0, end = sectionMeshes.size(); i < end; ++i ) {
        auto* const mesh = sectionMeshes.at( i );

        if( mesh != nullptr ) {
          auto quat = toQQuaternion( orientation );
          QVector3D positionCorrectionLeft = quat * QVector3D( 0, sectionOffsets.at( i * 2 ), 0 );
          QVector3D positionCorrectionRight = quat * QVector3D( 0, sectionOffsets.at( ( i * 2 ) + 1 ), 0 );

          Point_2 point2D = to2D( position );
          auto pointLeft = Point_2( point2D.x() + double( positionCorrectionLeft.x() ),
                                    point2D.y() + double( positionCorrectionLeft.y() ) );
          auto pointRight = Point_2( point2D.x() + double( positionCorrectionRight.x() ),
                                     point2D.y() + double( positionCorrectionRight.y() ) );

          mesh->addPoints( pointLeft, pointRight );

          if( mesh->vertexCount() > 1000 ) {
            qDebug() << "activeSectionsMeshes.at( i )->vertexCount() > 1000";
            mesh->optimise( threadForCgalWorker );
            sectionMeshes.at( i ) = createNewMesh();
            sectionMeshes.at( i )->addPoints( pointLeft, pointRight );
          }
        }
      }
    }
  }
}

void CultivatedAreaModel::setImplement( const QPointer<Implement>& implement ) {
  if( implement != nullptr ) {
    this->implement = implement;
    size_t numSections = implement->sections.size();

    for( auto* mesh : sectionMeshes ) {
      if( mesh != nullptr && mesh->vertexCount() > 3 ) {
        mesh->optimise( threadForCgalWorker );
      } else {
        mesh->deleteLater();
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

CultivatedAreaMesh* CultivatedAreaModel::createNewMesh() {
  auto* entity = new Qt3DCore::QEntity( m_baseEntity );

  entity->addComponent( m_phongMaterial );

  auto* mesh = new CultivatedAreaMesh( entity );
  entity->addComponent( mesh );

  return mesh;
}

void CultivatedAreaModel::setSections() {
  if( implement != nullptr ) {
    size_t numSections = implement->sections.size();

    const auto& section0 = implement->sections.at( 0 );
    const auto& state0 = section0->state();
    const bool globalForceOff = state0.testFlag( ImplementSection::State::ForceOff );
    const bool globalForceOn = state0.testFlag( ImplementSection::State::ForceOn );

    for( size_t i = 1; i < numSections; ++i ) {
      size_t sectionIndex = i - 1;
      const auto& section = implement->sections.at( i );
      const auto& state = section->state();

      if( !globalForceOff &&
          ( !state.testFlag( ImplementSection::State::ForceOff ) ) &&
          ( globalForceOn || section->isSectionOn() ) ) {
        if( sectionMeshes.at( sectionIndex ) == nullptr ) {
          sectionMeshes.at( sectionIndex ) = createNewMesh();
        }
      } else {
        if( sectionMeshes.at( sectionIndex ) != nullptr ) {
          if( sectionMeshes.at( sectionIndex )->vertexCount() > 3 ) {
            sectionMeshes.at( sectionIndex )->optimise( threadForCgalWorker );
          } else {
            sectionMeshes.at( sectionIndex )->deleteLater();
          }

          sectionMeshes.at( sectionIndex ) = nullptr;
        }
      }
    }
  }
}

CultivatedAreaModelFactory::CultivatedAreaModelFactory( Qt3DCore::QEntity* rootEntity, bool usePBR )
  : rootEntity( rootEntity ),
    usePBR( usePBR ),
    threadForCgalWorker( new CgalThread( this ) ) {
  threadForCgalWorker->start();
}

#include "moc_CultivatedAreaModel.cpp"
