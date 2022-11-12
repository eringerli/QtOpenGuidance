// Copyright( C ) 2021 Christian Riggenbach
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
