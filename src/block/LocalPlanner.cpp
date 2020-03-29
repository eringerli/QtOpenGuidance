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

#include "LocalPlanner.h"

#include "../cgal.h"

#include <dubins/dubins.h>

void LocalPlanner::setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    this->position = position;
    this->orientation = orientation;

    const Point_2 position2D = to2D( position );

    if( !turningLeft && !turningRight ) {
      if( !globalPlan.plan->empty() ) {

        // local planner for lines: find the nearest line and put it into the local plan
        double distanceSquared = qInf();
        auto nearestPrimitive = globalPlan.getNearestPrimitive( position2D, distanceSquared );
        double distanceNearestPrimitive = std::sqrt( distanceSquared );

        if( !lastPrimitive || distanceNearestPrimitive < ( std::sqrt( lastPrimitive->distanceToPointSquared( position2D ) ) - pathHysteresis ) ) {
          lastPrimitive = *nearestPrimitive;
          plan.type = globalPlan.type;
          plan.plan->clear();
        }

        if( lastPrimitive->anyDirection ) {
          double angleLastPrimitiveDegrees = lastPrimitive->angleAtPointDegrees( position2D );
          double steerAngleAbsoluteDegrees = steeringAngleDegrees + orientation.toEulerAngles().z();

          qDebug() << angleLastPrimitiveDegrees << steerAngleAbsoluteDegrees << std::abs( std::abs( steerAngleAbsoluteDegrees ) - std::abs( angleLastPrimitiveDegrees ) );

          if( std::abs( std::abs( steerAngleAbsoluteDegrees ) - std::abs( angleLastPrimitiveDegrees ) ) > 95 ) {
            qDebug() << "std::abs(std::abs";
            auto reverse = lastPrimitive->createReverse();
            reverse->anyDirection = false;
            lastPrimitive = reverse;
            plan.type = globalPlan.type;
            plan.plan->clear();
          }
        }

        if( plan.plan->empty() ) {
          plan.plan->push_back( lastPrimitive );
          emit planChanged( plan );
        }
      }
    } else {
      if( std::distance( lastSegmentOfTurn, targetSegmentOfTurn ) < 3 && ( *targetSegmentOfTurn )->isOn( position2D ) ) {
        turningLeft = false;
        turningRight = false;
        emit resetTurningStateOfDock();

        m_segmentsEntity->setEnabled( false );

        setPose( position, orientation, options );
      }

      {
        double squaredDistance = 0;
        lastSegmentOfTurn = plan.getNearestPrimitive( position2D, squaredDistance );
      }
    }
  }
}

void LocalPlanner::setPlan( const Plan& plan ) {
  this->globalPlan = plan;
  lastPrimitive = nullptr;
}

void LocalPlanner::turnLeftToggled( bool state ) {
  qDebug() << "LocalPlanner::turnLeftToggled" << state << turningLeft;

  if( state == true ) {
    bool existingTurn = turningLeft | turningRight;
    turningLeft = true;
    turningRight = false;
    calculateTurning( existingTurn );
  } else {
    turningLeft = false;
    turningRight = false;
  }
}

void LocalPlanner::turnRightToggled( bool state ) {
  qDebug() << "LocalPlanner::turnRightToggled" << state << turningRight;

  if( state == true ) {
    bool existingTurn = turningLeft | turningRight;
    turningRight = true;
    turningLeft = false;
    calculateTurning( existingTurn );
  } else {
    turningLeft = false;
    turningRight = false;
  }
}

void LocalPlanner::numSkipChanged( int left, int right ) {
  qDebug() << "LocalPlanner::numSkipChanged" << left << right;
  bool existingTurn = turningLeft | turningRight;
  leftSkip = left;
  rightSkip = right;

  if( existingTurn ) {
    calculateTurning( true );
  }
}

void LocalPlanner::calculateTurning( bool changeExistingTurn ) {
  if( !globalPlan.plan->empty() ) {
    const Point_2 position2D = to2D( position );
    const double heading = orientation.toEulerAngles().z();

    if( !changeExistingTurn ) {
      positionTurnStart = position2D;
      headingTurnStart = heading;
    }

    double distanceSquared = qInf();
    auto nearestPrimitive = globalPlan.getNearestPrimitive( positionTurnStart, distanceSquared );

    double angleNearestPrimitiveDegrees = ( *nearestPrimitive )->angleAtPointDegrees( positionTurnStart );
    bool searchUp = turningLeft;

    bool reversedLine = std::abs( headingTurnStart - angleNearestPrimitiveDegrees ) > 90;

    if( reversedLine ) {
      searchUp = !searchUp;
    }

    if( const auto line = ( *nearestPrimitive )->castToLine() ) {

      // trigger the calculation of the plan
      double angleOffsetRad = qDegreesToRadians( angleNearestPrimitiveDegrees );
      auto offset = polarOffsetRad( angleOffsetRad - M_PI, ( *nearestPrimitive )->implementWidth * ( turningLeft ? leftSkip : rightSkip ) );

      if( turningRight ) {
        offset = -offset;
      }

      if( reversedLine ) {
        offset = -offset;
      }

      emit triggerPlanPose( to3D( positionTurnStart + offset ), orientation, PoseOption::Options() );

      nearestPrimitive = globalPlan.getNearestPrimitive( positionTurnStart, distanceSquared );

      auto perpendicularLine = line->line.perpendicular( positionTurnStart );

      Plan::ConstPrimitiveIterator targetLineIt = globalPlan.plan->cend();

      if( turningLeft ) {
        targetLineIt = nearestPrimitive + ( searchUp ? leftSkip : -leftSkip );
      } else {
        targetLineIt = nearestPrimitive + ( searchUp ? rightSkip : -rightSkip );
      }

      if( targetLineIt != globalPlan.plan->cend() ) {
        Point_2 resultingPoint;

        if( ( *targetLineIt )->intersectWithLine( perpendicularLine, resultingPoint ) ) {
          double headingAtTargetRad = qDegreesToRadians( ( *targetLineIt )->angleAtPointDegrees( resultingPoint ) ) + ( reversedLine ? M_PI : 0 );

//          std::cout << "LocalPlanner::turnLeftToggled position2D:" << position2D << ", resultingPoint:" << resultingPoint << ", angleNearestPrimitive:" << angleNearestPrimitiveDegrees << "headingAtTarget" << headingAtTargetRad << std::endl;

          DubinsPath dubinsPath;
          double q0[] = {position2D.x(), position2D.y(), qDegreesToRadians( heading )};
          double q1[] = {resultingPoint.x(), resultingPoint.y(), headingAtTargetRad - M_PI};

          dubins_shortest_path( &dubinsPath, q0, q1, minRadius );

          std::vector<Point_2> polyline;

          dubins_path_sample_many( &dubinsPath, 0.2, []( double q[3], double /*t*/, void* user_data ) -> int{
            ( ( std::vector<Point_2>* )user_data )->push_back( Point_2( q[0], q[1] ) );
            return 0;
          },
          ( void* )( &polyline ) );

          std::vector<Point_2> optimizedPolyline;
          PS::Squared_distance_cost cost;
          PS::simplify( polyline.cbegin(), polyline.cend(), cost, PS::Stop_above_cost_threshold( 0.008 ), std::back_inserter( optimizedPolyline ) );

          if( optimizedPolyline.size() ) {
            plan.plan->clear();
            auto lastPoint = optimizedPolyline.cbegin();

            for( auto point = optimizedPolyline.cbegin() + 1, end = optimizedPolyline.cend(); point != end; ++point ) {
              plan.plan->push_back( std::make_shared<PathPrimitiveSegment>(
                                      Segment_2( *lastPoint, *point ),
                                      0, false, 0 ) );
              lastPoint = point;
            }

            auto offset = polarOffsetRad( headingAtTargetRad - M_PI_2 - M_PI, 100 );
            plan.plan->push_back( std::make_shared<PathPrimitiveSegment>(
                                    Segment_2( *lastPoint, ( *lastPoint ) - offset ),
                                    0, false, 0 ) );

            lastSegmentOfTurn = plan.plan->cbegin();
            targetSegmentOfTurn = --plan.plan->cend();

            {
              QVector<QVector3D> positions;

              for( auto primitive : *plan.plan ) {
                auto segment = primitive->castToSegment();

                positions << QVector3D( float( segment->segment.source().x() ), float( segment->segment.source().y() ), 1 );
                positions << QVector3D( float( segment->segment.target().x() ), float( segment->segment.target().y() ), 1 );
              }

              m_segmentsMesh->bufferUpdate( positions );
              m_segmentsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
              m_segmentsEntity->setEnabled( true );
            }

            plan.type = Plan::Type::Mixed;
            emit planChanged( plan );
          }
        }
      }
    }
  }
}
