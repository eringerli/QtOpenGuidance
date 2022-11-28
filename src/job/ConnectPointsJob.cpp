// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ConnectPointsJob.h"

#include <CGAL/point_generators_2.h>

ConnectPointsJob::ConnectPointsJob( std::shared_ptr< std::vector< Point_2 > > pointsPointer, double distanceBetweenConnectPoints )
    : pointsPointer( pointsPointer ), distanceBetweenConnectPoints( distanceBetweenConnectPoints ) {
  std::cout << "ConnectPointsJob::ConnectPointsJob" << std::endl;
}

void
ConnectPointsJob::run( ThreadWeaver::JobPointer, ThreadWeaver::Thread* ) {
  std::cout << "ConnectPointsJob::run" << std::endl;

  if( distanceBetweenConnectPoints > 0 && pointsPointer->size() >= 2 ) {
    for( auto last = pointsPointer->cbegin(), it = pointsPointer->cbegin() + 1, end = pointsPointer->cend(); it != end; ++it, ++last ) {
      const double distance = std::sqrt( CGAL::squared_distance( *last, *it ) );

      if( ( distance - 0.01 ) > distanceBetweenConnectPoints ) {
        std::size_t numPoints = std::min( distance / distanceBetweenConnectPoints, 1000. );

        CGAL::Points_on_segment_2< Epick::Point_2 > pointGenerator( *last, *it, numPoints );

        std::copy_n( pointGenerator, numPoints, std::back_inserter( *pointsPointer ) );
      }
    }
  }
}
