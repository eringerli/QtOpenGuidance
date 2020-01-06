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

#include "SprayerModel.h"

#include <QtCore/QDebug>
#include <QtMath>
#include <QElapsedTimer>

#include "../3d/BufferMesh.h"


SprayerModel::SprayerModel( Qt3DCore::QEntity* rootEntity ) {

  // add an entry, so all coordinates are local
  m_rootEntity = new Qt3DCore::QEntity( rootEntity );
  m_rootEntityTransform = new Qt3DCore::QTransform( m_rootEntity );
  m_rootEntity->addComponent( m_rootEntityTransform );
}

// order is important! Crashes if a parent entity is removed first!
SprayerModel::~SprayerModel() {
  m_rootEntityTransform->deleteLater();
  m_rootEntity->deleteLater();
}

void SprayerModel::setPose( const Point_3& position, const QQuaternion rotation, const PoseOption::Options options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    m_rootEntityTransform->setTranslation( convertPoint3ToQVector3D( position ) );
    m_rootEntityTransform->setRotation( rotation );
  }
}

void SprayerModel::setSections() {
  if( implement != nullptr ) {
    int numSections = implement->sections.size();
    --numSections;

    const auto& section0 = implement->sections.at( 0 );
    const auto& state0 = section0->state();
    const bool globalForceOff = state0.testFlag( ImplementSection::State::ForceOff );
    const bool globalForceOn = state0.testFlag( ImplementSection::State::ForceOn );

    for( int sectionIndex = 0; sectionIndex < numSections; ++sectionIndex ) {

      QElapsedTimer timer;
      timer.start();

      const auto& boomMaterial = boomMaterials.at( sectionIndex );
      const auto& sprayEntity = sprayEntities.at( sectionIndex );

      const auto& section = implement->sections.at( sectionIndex + 1 );
      const auto& state = section->state();

      if( state.testFlag( ImplementSection::State::ForceOff ) || globalForceOff ) {
        boomMaterial->setBaseColor( QColor( Qt::red ) );
        sprayEntity->setEnabled( false );
      } else {
        if( state.testFlag( ImplementSection::State::ForceOn ) || globalForceOn ) {
          boomMaterial->setBaseColor( QColor( Qt::green ) );
          sprayEntity->setEnabled( true );
        } else {
          if( section->isSectionOn() ) {
            boomMaterial->setBaseColor( QColor( Qt::darkGreen ) );
            sprayEntity->setEnabled( true );
          } else {
            boomMaterial->setBaseColor( QColor( Qt::darkRed ) );
            sprayEntity->setEnabled( false );
          }
        }
      }
    }
  }
}

void SprayerModel::setImplement( const QPointer<Implement>& implement ) {
  if( implement != nullptr ) {
    this->implement = implement;

    int numSections = implement->sections.size();
    --numSections;

    const auto& allEntityChildren = m_rootEntity->findChildren<Qt3DCore::QEntity*>( QString(), Qt::FindDirectChildrenOnly );

    if( numSections != allEntityChildren.count() && numSections > 0 ) {
      // delete old sections
      for( const auto& child : allEntityChildren ) {
        child->deleteLater();
      }

      boomMaterials.clear();
      sprayEntities.clear();

      // create new sections
      for( int i = 1; i <= numSections; ++i ) {
        auto entity = new Qt3DCore::QEntity( m_rootEntity );

        auto boomEntity = new Qt3DCore::QEntity( entity );
        auto boomMesh = new Qt3DExtras::QCylinderMesh( boomEntity );
        auto materialBoom = new Qt3DExtras::QMetalRoughMaterial( boomEntity );
        auto boomMeshTransform = new Qt3DCore::QTransform( boomEntity );

        boomMesh->setRadius( 0.2f );
        boomMesh->setRings( 10.0f );
        boomMesh->setSlices( 20.0f );
        materialBoom->setBaseColor( QColor( QRgb( 0x668823 ) ) );
        materialBoom->setMetalness( .3f );
        materialBoom->setRoughness( 0.2f );
//        boomMeshTransform->setRotation(
//              QQuaternion::fromAxisAndAngle(
//                QVector3D( 0.0f, 0.0f, 1.0f ),
//                90 ) );

        boomEntity->addComponent( boomMesh );
        boomEntity->addComponent( materialBoom );
        boomEntity->addComponent( boomMeshTransform );


        auto sprayEntity = new Qt3DCore::QEntity( entity );
        auto* sprayMesh = new BufferMesh( sprayEntity );
        auto sprayMaterial = new Qt3DExtras::QMetalRoughMaterial( sprayEntity );
        auto sprayMeshTransform = new Qt3DCore::QTransform( sprayEntity );

        sprayMaterial->setBaseColor( sprayerColor );
        sprayMaterial->setMetalness( .1f );
        sprayMaterial->setRoughness( 0.1f );

//        sprayMaterial->setAlphaBlendingEnabled( true );
//        sprayMaterial->setDiffuse( sprayerColor /*QColor( qRgba( 0xff, 0xff, 0xff, 20 ) )*/ );
//        sprayMaterial->setAmbient( QColor( qRgba( 127, 127, 127, 0 ) ) );
//        sprayMaterial->setShininess( 10 );
        sprayMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::TriangleFan );

        sprayEntity->addComponent( sprayMesh );
        sprayEntity->addComponent( sprayMaterial );
        sprayEntity->addComponent( sprayMeshTransform );

        boomMaterials.push_back( materialBoom );
        sprayEntities.push_back( sprayEntity );
      }
    }

    updateProprotions();

    setSections();
  }
}

void SprayerModel::setHeight( float height ) {
  this->m_height = height;
  updateProprotions();
}

void SprayerModel::updateProprotions() {
  if( implement != nullptr ) {

    int numSections = implement->sections.size();
    --numSections;

    // get the left most point of the implement
    double middleOfSection = 0;

    for( const auto& section : qAsConst( implement->sections ) ) {
      middleOfSection +=  section->widthOfSection - section->overlapLeft - section->overlapRight;
    }

    middleOfSection = middleOfSection / 2;

    // get all direct children of type Entity*
    auto childrenOfRootEntity = m_rootEntity->findChildren<Qt3DCore::QEntity*>( QString(), Qt::FindDirectChildrenOnly );

    if( numSections > 0 && childrenOfRootEntity.count() >= numSections ) {
      int sectionIndex = 1;
      // only operate on the [numSections] last sections
      auto entity = childrenOfRootEntity.cbegin();
      entity += childrenOfRootEntity.count() - numSections;
      auto end = childrenOfRootEntity.cend();

      for( ; entity != end; ++entity ) {
        const auto section = implement->sections.at( sectionIndex );
        middleOfSection += section->overlapLeft - section->widthOfSection;

        const auto entityVector = ( *entity )->findChildren<Qt3DCore::QEntity*>( QString(), Qt::FindDirectChildrenOnly );
        const auto boomEntity = entityVector.at( 0 );
        const auto sprayEntity = entityVector.at( 1 );

        if( ( boomEntity != nullptr ) && ( sprayEntity != nullptr ) ) {
          const auto boomMesh = boomEntity->componentsOfType<Qt3DExtras::QCylinderMesh>().constFirst();
          const auto boomMeshTransform = boomEntity->componentsOfType<Qt3DCore::QTransform>().constFirst();

          const auto sprayMesh = sprayEntity->componentsOfType<BufferMesh>().constFirst();
          const auto sprayMeshTransform = sprayEntity->componentsOfType<Qt3DCore::QTransform>().constFirst();

          if( ( boomMesh != nullptr ) && ( boomMeshTransform != nullptr ) && ( sprayMesh != nullptr ) && ( sprayMeshTransform != nullptr ) ) {

            boomMesh->setLength( float( section->widthOfSection ) );
            boomMeshTransform->setTranslation(
              QVector3D( 0, float( middleOfSection ) + ( boomMesh->length() / 2 ), m_height ) );
            sprayMeshTransform->setTranslation( boomMeshTransform->translation() );
            {
              QVector<QVector3D> sprayTrianglePoints;
              auto widthOfSectionHalf = float( section->widthOfSection / 2 );
              float heightThird = m_height / 3;
              sprayTrianglePoints << QVector3D( 0, 0, 0 );
              sprayTrianglePoints << QVector3D( -heightThird, widthOfSectionHalf, -m_height );
              sprayTrianglePoints << QVector3D( -heightThird, -widthOfSectionHalf, -m_height );

              sprayTrianglePoints << QVector3D( heightThird, -widthOfSectionHalf, -m_height );
              sprayTrianglePoints << QVector3D( heightThird, widthOfSectionHalf, -m_height );
              sprayTrianglePoints << QVector3D( -heightThird, widthOfSectionHalf, -m_height );

              sprayMesh->bufferUpdate( sprayTrianglePoints );
            }
          }
        }

        if( ++sectionIndex > numSections ) {
          break;
        }

        middleOfSection += section->overlapRight;
      }
    }
  }
}
