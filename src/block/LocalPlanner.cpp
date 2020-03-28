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

Plan::ConstPrimitiveIterator LocalPlanner::getNearestPrimitive( const Point_2& position2D, Plan plan, double& distanceSquared ) {
  Plan::ConstPrimitiveIterator nearestPrimitive = plan.plan->cend();
  distanceSquared = qInf();

  for( auto it = plan.plan->cbegin(), end = plan.plan->cend(); it != end; ++it ) {
    double currentDistanceSquared = ( *it )->distanceToPointSquared( position2D );

    if( currentDistanceSquared < distanceSquared ) {
      nearestPrimitive = it;
      distanceSquared = currentDistanceSquared;
    } else {
      if( globalPlan.type == Plan::Type::OnlyLines ) {
        // the plan is ordered, so we can take the fast way out...
        break;
      }
    }
  }

  return nearestPrimitive;
}

void LocalPlanner::setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    this->position = position;
    this->orientation = orientation;

    if( !turningLeft && !turningRight ) {

      const Point_2 position2D = to2D( position );

      if( !globalPlan.plan->empty() ) {

        // local planner for lines: find the nearest line and put it into the local plan
        double distanceSquared = qInf();
        auto nearestPrimitive = getNearestPrimitive( position2D, globalPlan, distanceSquared );

        plan.type = Plan::Type::OnlyLines;
        plan.plan->clear();

        if( ( *nearestPrimitive )->anyDirection ) {
          double angleNearestPrimitive = ( *nearestPrimitive )->angleAtPoint( position2D );

          if( std::abs( orientation.toEulerAngles().z() - angleNearestPrimitive ) > 95 ) {
            auto reverse = ( *nearestPrimitive )->createReverse();
            plan.plan->push_back( reverse );
          } else {
            plan.plan->push_back( *nearestPrimitive );
          }

          emit planChanged( plan );
        }
      }
    }
  }
}

void LocalPlanner::turnLeftToggled( bool state ) {
  qDebug() << "LocalPlanner::turnLeftToggled" << state << turningLeft;

//if(turningLeft)
  if( state == true ) {
    const Point_2 position2D = to2D( position );

    double distanceSquared = qInf();
    auto nearestPrimitive = getNearestPrimitive( position2D, globalPlan, distanceSquared );

    double angleNearestPrimitive = ( *nearestPrimitive )->angleAtPoint( position2D );
    bool searchUp = true;

    double heading = orientation.toEulerAngles().z();

    if( std::abs( heading - angleNearestPrimitive ) > 90 ) {
      searchUp = false;
    }

    if( const auto line = ( *nearestPrimitive )->castToLine() ) {
      auto perpendicularLine = line->line.perpendicular( position2D );

      auto targetLineIt = nearestPrimitive + ( searchUp ? leftSkip : -leftSkip );

      if( targetLineIt < globalPlan.plan->cend() ) {
        Point_2 resultingPoint;

        if( ( *targetLineIt )->intersectWithLine( perpendicularLine, resultingPoint ) ) {
          double headingAtTarget = ( *targetLineIt )->angleAtPoint( resultingPoint );

          std::cout << "LocalPlanner::turnLeftToggled position2D:" << position2D << ", resultingPoint:" << resultingPoint << ", angleNearestPrimitive:" << angleNearestPrimitive << "headingAtTarget" << headingAtTarget << std::endl;

          DubinsPath dubinsPath;
          double q0[] = {position2D.x(), position2D.y(), qDegreesToRadians( heading )};
          double q1[] = {resultingPoint.x(), resultingPoint.y(), qDegreesToRadians( headingAtTarget - 180 )};

          dubins_shortest_path( &dubinsPath, q0, q1, 15 );

          std::vector<Point_2> polyline;

          dubins_path_sample_many( &dubinsPath, 0.2, []( double q[3], double /*t*/, void* user_data ) -> int{
            ( ( std::vector<Point_2>* )user_data )->push_back( Point_2( q[0], q[1] ) );
            return 0;
          },
          ( void* )( &polyline ) );

          std::vector<Point_2> optimizedPolyline;
          PS::Squared_distance_cost cost;
          PS::simplify( polyline.cbegin(), polyline.cend(), cost, PS::Stop_above_cost_threshold( 0.01 ), std::back_inserter( optimizedPolyline ) );

          qDebug() << "sizes: " << polyline.size() << optimizedPolyline.size();

          QVector<QVector3D> positions;

          for( auto point : optimizedPolyline ) {
            positions << QVector3D( float( point.x() ), float( point.y() ), 0 );
          }

          m_segmentsMesh->bufferUpdate( positions );
          m_segmentsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );
          m_segmentsEntity->setEnabled( true );

          if( optimizedPolyline.size() ) {
            plan.plan->clear();
            auto lastPoint = optimizedPolyline.cbegin();

            for( auto point = optimizedPolyline.cbegin() + 1, end = optimizedPolyline.cend(); point != end; ++point ) {
              plan.plan->push_back( std::make_shared<PathPrimitiveSegment>(
                                      Segment_2( *lastPoint, *point ),
                                      0, false, 0 ) );
              lastPoint = point;
            }

            turningLeft = true;
            plan.type = Plan::Type::Mixed;
            qDebug() << "planChanged(plan)";
            emit planChanged( plan );
          }
        }
      }
    }
  }
}

void LocalPlanner::turnRightToggled( bool state ) {
  qDebug() << "LocalPlanner::turnRightToggled" << state << turningRight;

}

void LocalPlanner::numSkipChanged( int left, int right ) {
  qDebug() << "LocalPlanner::numSkipChanged" << left << right;
  leftSkip = left;
  rightSkip = right;
}
