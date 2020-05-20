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

#include "PathPlannerModel.h"

#include <QObject>

#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>
#include <Qt3DRender/QAttribute>

#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"


#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"

#include "../kinematic/Plan.h"

#include "../kinematic/PathPrimitive.h"
#include "../kinematic/PathPrimitiveLine.h"
#include "../kinematic/PathPrimitiveRay.h"
#include "../kinematic/PathPrimitiveSegment.h"
#include "../kinematic/PathPrimitiveSequence.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>


PathPlannerModel::PathPlannerModel( Qt3DCore::QEntity* rootEntity )
  : BlockBase(),
    rootEntity( rootEntity ) {

  baseEntity = new Qt3DCore::QEntity( rootEntity );

  linesEntity = new Qt3DCore::QEntity( baseEntity );
  raysEntity = new Qt3DCore::QEntity( baseEntity );
  segmentsEntity = new Qt3DCore::QEntity( baseEntity );
  bisectorsEntity = new Qt3DCore::QEntity( baseEntity );

  linesEntity->setEnabled( false );
  raysEntity->setEnabled( false );
  segmentsEntity->setEnabled( false );
  bisectorsEntity->setEnabled( false );

  linesMesh = new BufferMesh( linesEntity );
  linesMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
  linesEntity->addComponent( linesMesh );

  raysMesh = new BufferMesh( raysEntity );
  raysMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
  raysEntity->addComponent( raysMesh );

  segmentsMesh = new BufferMesh( segmentsEntity );
  segmentsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
  segmentsEntity->addComponent( segmentsMesh );

  bisectorsMesh = new BufferMesh( bisectorsEntity );
  bisectorsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
  bisectorsEntity->addComponent( bisectorsMesh );

  linesMaterial = new Qt3DExtras::QPhongMaterial( linesEntity );
  raysMaterial = new Qt3DExtras::QPhongMaterial( raysEntity );
  segmentsMaterial = new Qt3DExtras::QPhongMaterial( segmentsEntity );
  bisectorsMaterial = new Qt3DExtras::QPhongMaterial( bisectorsEntity );

  linesEntity->addComponent( linesMaterial );
  raysEntity->addComponent( raysMaterial );
  segmentsEntity->addComponent( segmentsMaterial );
  bisectorsEntity->addComponent( bisectorsMaterial );

  refreshColors();
}

void PathPlannerModel::refreshColors() {
  linesMaterial->setAmbient( linesColor );

  if( individualRayColor ) {
    raysMaterial->setAmbient( raysColor );
  } else {
    raysMaterial->setAmbient( linesColor );
  }

  if( individualSegmentColor ) {
    segmentsMaterial->setAmbient( segmentsColor );
  } else {
    segmentsMaterial->setAmbient( linesColor );
  }

  bisectorsMaterial->setAmbient( bisectorsColor );
}

void PathPlannerModel::setPose( const Point_3& position, const QQuaternion orientation, const PoseOption::Options options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    this->position = position;
    this->orientation = orientation;

    if( visible ) {

      if( !plan.plan->empty() ) {
        const Point_2 position2D = to2D( position );

        Iso_rectangle_2 viewBoxRect( Bbox_2( position2D.x() - viewBox, position2D.y() - viewBox, position2D.x() + viewBox, position2D.y() + viewBox ) );

        QVector<QVector3D> positionsLines;
        QVector<QVector3D> positionsRays;
        QVector<QVector3D> positionsSegments;
        QVector<QVector3D> positionsBisectors;

        for( const auto& step : * ( plan.plan ) ) {
          if( const auto* pathLine = step->castToLine() ) {
            const auto& line = pathLine->line;

            CGAL::cpp11::result_of<Intersect_2( Segment_2, Line_2 )>::type
            result = intersection( viewBoxRect, line );

            if( result ) {
              if( const Segment_2* segment = boost::get<Segment_2>( &*result ) ) {
                positionsLines << QVector3D( segment->source().x(), segment->source().y(), zOffset );
                positionsLines << QVector3D( segment->target().x(), segment->target().y(), zOffset );
              }
            }
          }

          if( const auto* pathSegment = step->castToSegment() ) {
            const auto& segment = pathSegment->segment;

            CGAL::cpp11::result_of<Intersect_2( Segment_2, Line_2 )>::type
            result = intersection( viewBoxRect, segment );

            if( result ) {
              if( const Segment_2* segment = boost::get<Segment_2>( &*result ) ) {
                positionsSegments << QVector3D( segment->source().x(), segment->source().y(), zOffset );
                positionsSegments << QVector3D( segment->target().x(), segment->target().y(), zOffset );
              }
            }
          }

          if( const auto* pathRay = step->castToRay() ) {
            const auto& ray = pathRay->ray;

            CGAL::cpp11::result_of<Intersect_2( Segment_2, Line_2 )>::type
            result = intersection( viewBoxRect, ray );

            if( result ) {
              if( const Segment_2* segment = boost::get<Segment_2>( &*result ) ) {
                positionsRays << QVector3D( segment->source().x(), segment->source().y(), zOffset );
                positionsRays << QVector3D( segment->target().x(), segment->target().y(), zOffset );
              }
            }
          }

          if( const auto* pathSequence = step->castToSequence() ) {
            for( const auto& step : pathSequence->sequence ) {
              if( const auto* pathLine = step->castToLine() ) {
                const auto& line = pathLine->line;

                CGAL::cpp11::result_of<Intersect_2( Segment_2, Line_2 )>::type
                result = intersection( viewBoxRect, line );

                if( result ) {
                  if( const Segment_2* segment = boost::get<Segment_2>( &*result ) ) {
                    positionsLines << QVector3D( segment->source().x(), segment->source().y(), zOffset );
                    positionsLines << QVector3D( segment->target().x(), segment->target().y(), zOffset );
                  }
                }
              }

              if( const auto* pathSegment = step->castToSegment() ) {
                const auto& segment = pathSegment->segment;

                CGAL::cpp11::result_of<Intersect_2( Segment_2, Line_2 )>::type
                result = intersection( viewBoxRect, segment );

                if( result ) {
                  if( const Segment_2* segment = boost::get<Segment_2>( &*result ) ) {
                    positionsSegments << QVector3D( segment->source().x(), segment->source().y(), zOffset );
                    positionsSegments << QVector3D( segment->target().x(), segment->target().y(), zOffset );
                  }
                }
              }

              if( const auto* pathRay = step->castToRay() ) {
                const auto& ray = pathRay->ray;

                CGAL::cpp11::result_of<Intersect_2( Segment_2, Line_2 )>::type
                result = intersection( viewBoxRect, ray );

                if( result ) {
                  if( const Segment_2* segment = boost::get<Segment_2>( &*result ) ) {
                    positionsRays << QVector3D( segment->source().x(), segment->source().y(), zOffset );
                    positionsRays << QVector3D( segment->target().x(), segment->target().y(), zOffset );
                  }
                }
              }
            }

            if( bisectorsVisible ) {
              for( const auto& line : pathSequence->bisectors ) {
                CGAL::cpp11::result_of<Intersect_2( Segment_2, Line_2 )>::type
                result = intersection( viewBoxRect, line );

                if( result ) {
                  //            positions3.clear();
                  if( const Segment_2* segment = boost::get<Segment_2>( &*result ) ) {
                    positionsBisectors << QVector3D( segment->source().x(), segment->source().y(), zOffset );
                    positionsBisectors << QVector3D( segment->target().x(), segment->target().y(), zOffset );
                  }
                }
              }
            }
          }
        }

        if( !positionsLines.isEmpty() ) {
          linesMesh->bufferUpdate( positionsLines );
          linesEntity->setEnabled( true );
        } else {
          linesEntity->setEnabled( false );
        }

        if( !positionsRays.isEmpty() ) {
          raysMesh->bufferUpdate( positionsRays );
          raysEntity->setEnabled( true );
        } else {
          raysEntity->setEnabled( false );
        }

        if( !positionsSegments.isEmpty() ) {
          segmentsMesh->bufferUpdate( positionsSegments );
          segmentsEntity->setEnabled( true );
        } else {
          segmentsEntity->setEnabled( false );
        }

        if( bisectorsVisible && !positionsBisectors.isEmpty() ) {
          bisectorsMesh->bufferUpdate( positionsBisectors );
          bisectorsEntity->setEnabled( true );
        } else {
          bisectorsEntity->setEnabled( false );
        }

        baseEntity->setEnabled( true );
      }
    } else {
      baseEntity->setEnabled( false );
    }
  }
}
