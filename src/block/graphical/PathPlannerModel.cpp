// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PathPlannerModel.h"

#include <QObject>

#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>

#include <QDebug>

#include "CGAL/Kernel/global_functions_3.h"
#include "CGAL/enum.h"
#include "CGAL/number_utils_classes.h"
#include "block/BlockBase.h"

#include "3d/BufferMesh.h"

#include "helpers/cgalHelper.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/PathPrimitiveArc.h"
#include "kinematic/PathPrimitiveLine.h"
#include "kinematic/PathPrimitiveRay.h"
#include "kinematic/PathPrimitiveSegment.h"
#include "kinematic/PathPrimitiveSequence.h"

#include <QJsonObject>
#include <QSharedPointer>
#include <QVector>
#include <cmath>
#include <functional>
#include <utility>

PathPlannerModel::PathPlannerModel( Qt3DCore::QEntity*         rootEntity,
                                    const BlockBaseId          idHint,
                                    const bool                 systemBlock,
                                    const QString              type,
                                    const BlockBase::TypeColor typeColor )
    : BlockBase( idHint, systemBlock, type, typeColor ) {
  baseEntity          = new Qt3DCore::QEntity( rootEntity );
  baseEntityTransform = new Qt3DCore::QTransform( baseEntity );
  baseEntityTransform->setTranslation( QVector3D( 0, 0, 0 ) );
  baseEntity->addComponent( baseEntityTransform );

  arcEntity       = new Qt3DCore::QEntity( baseEntity );
  linesEntity     = new Qt3DCore::QEntity( baseEntity );
  raysEntity      = new Qt3DCore::QEntity( baseEntity );
  segmentsEntity  = new Qt3DCore::QEntity( baseEntity );
  bisectorsEntity = new Qt3DCore::QEntity( baseEntity );

  arcEntity->setEnabled( false );
  linesEntity->setEnabled( false );
  raysEntity->setEnabled( false );
  segmentsEntity->setEnabled( false );
  bisectorsEntity->setEnabled( false );

  arcMesh = new BufferMesh( arcEntity );
  arcMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
  arcEntity->addComponent( arcMesh );

  linesMesh = new BufferMesh( linesEntity );
  linesMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
  linesEntity->addComponent( linesMesh );

  raysMesh = new BufferMesh( raysEntity );
  raysMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
  raysEntity->addComponent( raysMesh );

  segmentsMesh = new BufferMesh( segmentsEntity );
  segmentsMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
  segmentsEntity->addComponent( segmentsMesh );

  bisectorsMesh = new BufferMesh( bisectorsEntity );
  bisectorsMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
  bisectorsEntity->addComponent( bisectorsMesh );

  arcMaterial       = new Qt3DExtras::QPhongMaterial( arcEntity );
  linesMaterial     = new Qt3DExtras::QPhongMaterial( linesEntity );
  raysMaterial      = new Qt3DExtras::QPhongMaterial( raysEntity );
  segmentsMaterial  = new Qt3DExtras::QPhongMaterial( segmentsEntity );
  bisectorsMaterial = new Qt3DExtras::QPhongMaterial( bisectorsEntity );

  arcEntity->addComponent( arcMaterial );
  linesEntity->addComponent( linesMaterial );
  raysEntity->addComponent( raysMaterial );
  segmentsEntity->addComponent( segmentsMaterial );
  bisectorsEntity->addComponent( bisectorsMaterial );

  refreshColors();
}

void
PathPlannerModel::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "visible" )]                = visible;
  valuesObject[QStringLiteral( "zOffset" )]                = zOffset;
  valuesObject[QStringLiteral( "viewBox" )]                = viewBox;
  valuesObject[QStringLiteral( "individualArcColor" )]     = individualArcColor;
  valuesObject[QStringLiteral( "individualSegmentColor" )] = individualSegmentColor;
  valuesObject[QStringLiteral( "individualRayColor" )]     = individualRayColor;
  valuesObject[QStringLiteral( "bisectorsVisible" )]       = bisectorsVisible;
  valuesObject[QStringLiteral( "arcColor" )]               = arcColor.name();
  valuesObject[QStringLiteral( "lineColor" )]              = linesColor.name();
  valuesObject[QStringLiteral( "segmentColor" )]           = segmentsColor.name();
  valuesObject[QStringLiteral( "rayColor" )]               = raysColor.name();
  valuesObject[QStringLiteral( "bisectorsColor" )]         = bisectorsColor.name();
}

void
PathPlannerModel::fromJSON( const QJsonObject& valuesObject ) {
  visible                = valuesObject[QStringLiteral( "visible" )].toBool( true );
  zOffset                = valuesObject[QStringLiteral( "zOffset" )].toDouble( 0.1 );
  viewBox                = valuesObject[QStringLiteral( "viewBox" )].toDouble( 50 );
  individualArcColor     = valuesObject[QStringLiteral( "individualArcColor" )].toBool( false );
  individualSegmentColor = valuesObject[QStringLiteral( "individualSegmentColor" )].toBool( false );
  individualRayColor     = valuesObject[QStringLiteral( "individualRayColor" )].toBool( false );
  bisectorsVisible       = valuesObject[QStringLiteral( "bisectorsVisible" )].toBool( false );

  arcColor       = QColor( valuesObject[QStringLiteral( "arcColor" )].toString( QStringLiteral( "#00ff00" ) ) );
  linesColor     = QColor( valuesObject[QStringLiteral( "lineColor" )].toString( QStringLiteral( "#00ff00" ) ) );
  segmentsColor  = QColor( valuesObject[QStringLiteral( "segmentColor" )].toString( QStringLiteral( "#00ff00" ) ) );
  raysColor      = QColor( valuesObject[QStringLiteral( "rayColor" )].toString( QStringLiteral( "#00ff00" ) ) );
  bisectorsColor = QColor( valuesObject[QStringLiteral( "bisectorsColor" )].toString( QStringLiteral( "#0000ff" ) ) );

  refreshColors();
}

void
PathPlannerModel::refreshColors() {
  linesMaterial->setAmbient( linesColor );

  if( individualArcColor ) {
    arcMaterial->setAmbient( arcColor );
  } else {
    arcMaterial->setAmbient( linesColor );
  }

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

void
PathPlannerModel::setVisible( const bool visible ) {
  this->visible = visible;
  baseEntity->setEnabled( visible );
}

void
PathPlannerModel::enable( const bool enable ) {
  BlockBase::enable( enable );

  setVisible( enable );
}

void
PathPlannerModel::setPlan( const Plan& plan ) {
  this->plan   = plan;
  newPlan      = true;
  arcsBuffered = false;
}

bool
PathPlannerModel::getSegmentForPlanPrimitive( const Plan::PrimitiveSharedPointer& primitive,
                                              const Iso_rectangle_2&              viewBoxRect,
                                              Segment_2&                          segmentOut ) {
  if( const auto* pathLine = primitive->castToLine() ) {
    const auto& line = pathLine->line;

    auto result = intersection( viewBoxRect, line );

    if( result ) {
      if( const Segment_2* segment = boost::get< Segment_2 >( &*result ) ) {
        segmentOut = *segment;
        return true;
      }
    }
  }

  if( const auto* pathRay = primitive->castToRay() ) {
    const auto& ray = pathRay->ray;

    auto result = intersection( viewBoxRect, ray );

    if( result ) {
      if( const Segment_2* segment = boost::get< Segment_2 >( &*result ) ) {
        segmentOut = *segment;
        return true;
      }
    }
  }

  if( const auto* pathSegment = primitive->castToSegment() ) {
    const auto& segment = pathSegment->segment;

    auto result = intersection( viewBoxRect, segment );

    if( result ) {
      if( const Segment_2* segment = boost::get< Segment_2 >( &*result ) ) {
        segmentOut = *segment;
        return true;
      }
    }
  }

  return false;
}

void
PathPlannerModel::addSegmentToPositionsVector( const Segment_2& segment, QVector< QVector3D >& positions ) {
  positions << QVector3D( ( float )segment.source().x(), ( float )segment.source().y(), ( float )zOffset );
  positions << QVector3D( ( float )segment.target().x(), ( float )segment.target().y(), ( float )zOffset );
}

void
PathPlannerModel::addPrimitiveToPositions( const Plan::PrimitiveSharedPointer& primitive,
                                           const Iso_rectangle_2&              viewBoxRect,
                                           QVector< QVector3D >&               positionsArcs,
                                           QVector< QVector3D >&               positionsLines,
                                           QVector< QVector3D >&               positionsRays,
                                           QVector< QVector3D >&               positionsSegments ) {
  if( const auto* arc = primitive->castToArc() ) {
    if( newPlan ) {
      const auto& center = arc->centerPoint;

      auto startToCenterLine =
        Line_2( Point_2( CGAL::to_double( arc->arc.source().x() ), CGAL::to_double( arc->arc.source().y() ) ), arc->centerPoint );
      auto endToCenterLine =
        Line_2( Point_2( CGAL::to_double( arc->arc.target().x() ), CGAL::to_double( arc->arc.target().y() ) ), arc->centerPoint );

      auto angleStartRad = angleOfLineRadians( startToCenterLine ) + M_PI;
      auto angleEndRad   = angleOfLineRadians( endToCenterLine ) + M_PI;

      auto lastPosition = QVector3D();

      angleStartRad = normalizeAngleRadians( angleStartRad );
      angleEndRad   = normalizeAngleRadians( angleEndRad );

      if( std::abs( angleStartRad - angleEndRad ) > M_PI ) {
        std::swap( startToCenterLine, endToCenterLine );
        angleStartRad = angleOfLineRadians( startToCenterLine ) + M_PI;
        angleEndRad   = angleOfLineRadians( endToCenterLine ) + M_PI;
        lastPosition  = QVector3D(
          ( float )CGAL::to_double( arc->arc.target().x() ), ( float )CGAL::to_double( arc->arc.target().y() ), ( float )zOffset );
      } else {
        lastPosition = QVector3D(
          ( float )CGAL::to_double( arc->arc.source().x() ), ( float )CGAL::to_double( arc->arc.source().y() ), ( float )zOffset );
      }

      // radius / 0.08 ^= a point every 25cm
      auto angleStep = ( angleEndRad - angleStartRad ) / ( arc->radius / 0.08 );

      bool ( *comparisonLessThan )( double, double )    = []( double i, double j ) -> bool { return i < j; };
      bool ( *comparisonGreaterThan )( double, double ) = []( double i, double j ) -> bool { return i > j; };

      auto comparison = comparisonGreaterThan;
      if( !std::signbit( angleStep ) ) {
        comparison = comparisonLessThan;
      }

      {
        double angle = angleStartRad;
        for( auto comparison =
               ( ( !std::signbit( angleStep ) ) ? []( double i, double j ) { return i < j; } : []( double i, double j ) { return i > j; } );
             comparison( angle, angleEndRad );
             angle += angleStep ) {
          auto position = QVector3D( ( float )( center.x() + ( cos( angle ) * ( arc->radius ) ) ),
                                     ( float )( center.y() + ( sin( angle ) * ( arc->radius ) ) ),
                                     ( float )zOffset );
          positionsArcs << lastPosition << position;
          lastPosition = position;
        }
      }
    }
  } else {
    Segment_2 segment;
    if( getSegmentForPlanPrimitive( primitive, viewBoxRect, segment ) ) {
      switch( primitive->getType() ) {
        case PathPrimitive::Type::Line: {
          addSegmentToPositionsVector( segment, positionsLines );
        } break;

        case PathPrimitive::Type::Ray: {
          addSegmentToPositionsVector( segment, positionsRays );
        } break;

        case PathPrimitive::Type::Segment: {
          addSegmentToPositionsVector( segment, positionsSegments );
        } break;

        default:
          break;
      }
    }
  }
}

void
PathPlannerModel::setPose( const Eigen::Vector3d&           position,
                           const Eigen::Quaterniond&        orientation,
                           const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::Graphical ) ) {
    this->position    = position;
    this->orientation = orientation;
    baseEntityTransform->setTranslation( QVector3D( 0, 0, position.z() ) );

    if( visible ) {
      if( !plan.plan->empty() ) {
        const Point_2 position2D = to2D( position );

        Iso_rectangle_2 viewBoxRect(
          Bbox_2( position2D.x() - viewBox, position2D.y() - viewBox, position2D.x() + viewBox, position2D.y() + viewBox ) );

        QVector< QVector3D > positionsArcs;
        QVector< QVector3D > positionsLines;
        QVector< QVector3D > positionsRays;
        QVector< QVector3D > positionsSegments;
        QVector< QVector3D > positionsBisectors;

        for( const auto& step : *( plan.plan ) ) {
          if( step->getType() == PathPrimitive::Type::Sequence ) {
            const auto* pathSequence = step->castToSequence();
            for( const auto& step : pathSequence->sequence ) {
              addPrimitiveToPositions( step, viewBoxRect, positionsArcs, positionsLines, positionsRays, positionsSegments );

              if( bisectorsVisible ) {
                for( const auto& line : pathSequence->bisectors ) {
                  auto result = intersection( viewBoxRect, line );

                  if( result ) {
                    if( const Segment_2* segment = boost::get< Segment_2 >( &*result ) ) {
                      positionsBisectors << QVector3D( segment->source().x(), segment->source().y(), zOffset );
                      positionsBisectors << QVector3D( segment->target().x(), segment->target().y(), zOffset );
                    }
                  }
                }
              }
            }
          } else {
            addPrimitiveToPositions( step, viewBoxRect, positionsArcs, positionsLines, positionsRays, positionsSegments );
          }
        }

        if( !positionsArcs.isEmpty() ) {
          arcMesh->bufferUpdate( positionsArcs );
          arcsBuffered = true;
        }

        if( arcsBuffered ) {
          arcEntity->setEnabled( true );
        } else {
          arcEntity->setEnabled( false );
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

    newPlan = false;
  }
}

std::unique_ptr< BlockBase >
PathPlannerModelFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< PathPlannerModel >( idHint, rootEntity );

  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Plan" ), obj.get(), QLatin1StringView( SLOT( setPlan( const Plan& ) ) ) );

  return obj;
}
