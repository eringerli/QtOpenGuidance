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

#include "../3d/BufferMesh.h"


SprayerModel::SprayerModel( Qt3DCore::QEntity* rootEntity )
  : BlockBase() {

  // add an etry, so all coordinates are local
  m_rootEntity = new Qt3DCore::QEntity( rootEntity );
  m_rootEntityTransform = new Qt3DCore::QTransform( m_rootEntity );
  m_rootEntity->addComponent( m_rootEntityTransform );
}

// order is important! Crashes if a parent entity is removed first!
SprayerModel::~SprayerModel() {
  m_rootEntityTransform->deleteLater();
  m_rootEntity->deleteLater();
}

void SprayerModel::setPose( Point_3 position, QQuaternion rotation, PoseOption::Options options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    m_rootEntityTransform->setTranslation( convertPoint3ToQVector3D( position ) );
    m_rootEntityTransform->setRotation( rotation );
  }
}

void SprayerModel::setSections( QPointer<Implement> implement ) {
  if( implement ) {
    this->implement = implement;

    int numSections = implement->sections.count();
    --numSections;

    auto childrenOfRootEntity = m_rootEntity->findChildren<Qt3DCore::QEntity*>( QString(), Qt::FindDirectChildrenOnly );

    if( numSections > 0 && childrenOfRootEntity.count() >= numSections ) {
      // go through all the sections to seth the color and visibility
      // order is retained in children() -> use that
      int sectionIndex = numSections;
      bool globalForceOff = implement->sections.at( 0 )->state().testFlag( ImplementSection::State::ForceOff );
      bool globalForceOn = implement->sections.at( 0 )->state().testFlag( ImplementSection::State::ForceOn );

      for( auto child = childrenOfRootEntity.crbegin(); child != childrenOfRootEntity.crend(); ++child ) {
        Qt3DCore::QEntity* entity = qobject_cast<Qt3DCore::QEntity*>( *child );

        if( entity ) {
          auto entityVector = entity->findChildren<Qt3DCore::QEntity*>( QString(), Qt::FindDirectChildrenOnly );
          Qt3DCore::QEntity* boomEntity = entityVector.at( 0 );
          Qt3DCore::QEntity* sprayEntity = entityVector.at( 1 );

          if( boomEntity && sprayEntity ) {
            auto boomMaterial = boomEntity->findChildren<Qt3DExtras::QMetalRoughMaterial*>( QString(), Qt::FindDirectChildrenOnly ).first();

            if( boomMaterial ) {
              // set the color and spray according to the state
              auto state = implement->sections.at( sectionIndex )->state();

              if( state.testFlag( ImplementSection::State::ForceOff ) || globalForceOff ) {
                boomMaterial->setBaseColor( QColor( Qt::red ) );
                sprayEntity->setEnabled( false );
              } else {
                if( state.testFlag( ImplementSection::State::ForceOn ) || globalForceOn ) {
                  boomMaterial->setBaseColor( QColor( Qt::green ) );
                  sprayEntity->setEnabled( true );
                } else {
                  if( implement->sections.at( sectionIndex )->isSectionOn() ) {
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

        --sectionIndex;

        if( !sectionIndex ) {
          break;
        }
      }
    }
  }
}

void SprayerModel::setImplement( QPointer<Implement> implement ) {
  if( implement ) {
    this->implement = implement;

    int numSections = implement->sections.count();
    --numSections;

    if( numSections != m_rootEntity->findChildren<Qt3DCore::QEntity*>( QString(), Qt::FindDirectChildrenOnly ).count() && numSections > 0 ) {
      // delete old sections
      for( auto child : m_rootEntity->children() ) {
        Qt3DCore::QEntity* entity = qobject_cast<Qt3DCore::QEntity*>( child );

        if( entity ) {
          entity->deleteLater();
        }
      }

      // create new sections
      for( int i = 1; i <= numSections; ++i ) {
        Qt3DCore::QEntity* entity = new Qt3DCore::QEntity( m_rootEntity );

        Qt3DCore::QEntity* boomEntity = new Qt3DCore::QEntity( entity );
        Qt3DExtras::QCylinderMesh* boomMesh = new Qt3DExtras::QCylinderMesh( boomEntity );
        auto materialBoom = new Qt3DExtras::QMetalRoughMaterial( boomEntity );
        Qt3DCore::QTransform* boomMeshTransform = new Qt3DCore::QTransform( boomEntity );

        boomMesh->setRadius( 0.2f );
        boomMesh->setRings( 10.0f );
        boomMesh->setSlices( 20.0f );
        materialBoom->setBaseColor( QColor( QRgb( 0x668823 ) ) );
        materialBoom->setMetalness(.3f );
        materialBoom->setRoughness( 0.2f );
//        boomMeshTransform->setRotation(
//              QQuaternion::fromAxisAndAngle(
//                QVector3D( 0.0f, 0.0f, 1.0f ),
//                90 ) );

        boomEntity->addComponent( boomMesh );
        boomEntity->addComponent( materialBoom );
        boomEntity->addComponent( boomMeshTransform );


        Qt3DCore::QEntity* sprayEntity = new Qt3DCore::QEntity( entity );
        BufferMesh* sprayMesh = new BufferMesh( sprayEntity );
        auto sprayMaterial = new Qt3DExtras::QMetalRoughMaterial( sprayEntity );
        Qt3DCore::QTransform* sprayMeshTransform = new Qt3DCore::QTransform( sprayEntity );

        sprayMaterial->setBaseColor(sprayerColor);
        sprayMaterial->setMetalness(.1f );
        sprayMaterial->setRoughness( 0.1f );

//        sprayMaterial->setAlphaBlendingEnabled( true );
//        sprayMaterial->setDiffuse( sprayerColor /*QColor( qRgba( 0xff, 0xff, 0xff, 20 ) )*/ );
//        sprayMaterial->setAmbient( QColor( qRgba( 127, 127, 127, 0 ) ) );
//        sprayMaterial->setShininess( 10 );
        sprayMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::TriangleFan );

        sprayEntity->addComponent( sprayMesh );
        sprayEntity->addComponent( sprayMaterial );
        sprayEntity->addComponent( sprayMeshTransform );
      }
    }

    updateProprotions();
    setSections( implement );
  }
}

void SprayerModel::setHeight( float height ) {
  this->m_height = height;
  updateProprotions();
}

void SprayerModel::updateProprotions() {
  if( implement ) {

    int numSections = implement->sections.count();
    --numSections;

    // get the left most point of the implement
    double middleOfSection = 0;

    for( auto section : implement->sections ) {
      middleOfSection +=  section->widthOfSection - section->overlapLeft - section->overlapRight;
    }

    middleOfSection = middleOfSection / 2;

    // get all direct children of type Entity*
    auto childrenOfRootEntity = m_rootEntity->findChildren<Qt3DCore::QEntity*>( QString(), Qt::FindDirectChildrenOnly );

    if( numSections > 0 && childrenOfRootEntity.count() >= numSections ) {
      int sectionIndex = 1;
      // only operate on the [numSections] last sections
      auto child = childrenOfRootEntity.cbegin();
      child += childrenOfRootEntity.count() - numSections;
      auto end = childrenOfRootEntity.cend();

      for( ; child != end; ++child ) {
        Qt3DCore::QEntity* entity = qobject_cast<Qt3DCore::QEntity*>( *child );
        const auto section = implement->sections.at( sectionIndex );
        middleOfSection += section->overlapLeft - section->widthOfSection;

        if( entity ) {
          auto entityVector = entity->findChildren<Qt3DCore::QEntity*>( QString(), Qt::FindDirectChildrenOnly );
          Qt3DCore::QEntity* boomEntity = entityVector.at( 0 );
          Qt3DCore::QEntity* sprayEntity = entityVector.at( 1 );

          if( boomEntity && sprayEntity ) {
            Qt3DExtras::QCylinderMesh* boomMesh = boomEntity->findChildren<Qt3DExtras::QCylinderMesh*>( QString(), Qt::FindDirectChildrenOnly ).first();
            Qt3DCore::QTransform* boomMeshTransform = boomEntity->findChildren<Qt3DCore::QTransform*>( QString(), Qt::FindDirectChildrenOnly ).first();

            BufferMesh* sprayMesh = sprayEntity->findChildren<BufferMesh*>( QString(), Qt::FindDirectChildrenOnly ).first();
            Qt3DCore::QTransform* sprayMeshTransform = sprayEntity->findChildren<Qt3DCore::QTransform*>( QString(), Qt::FindDirectChildrenOnly ).first();

            if( boomMesh && boomMeshTransform && sprayMesh && sprayMeshTransform ) {

              boomMesh->setLength( float( section->widthOfSection ) );
              boomMeshTransform->setTranslation(
                QVector3D( 0, float( middleOfSection ) + ( boomMesh->length() / 2 ), m_height ) );
              sprayMeshTransform->setTranslation( boomMeshTransform->translation() );
              {
                QVector<QVector3D> sprayTrianglePoints;
                float widthOfSectionHalf = float( section->widthOfSection / 2 );
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
        }

        if( ++sectionIndex > numSections ) {
          break;
        }

        middleOfSection += section->overlapRight;
      }
    }
  }
}
