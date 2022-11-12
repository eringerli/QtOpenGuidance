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

#include "TestCollinearityJob.h"
#include <ThreadWeaver/Exception>

#include <CGAL/point_generators_2.h>

TestCollinearityJob::TestCollinearityJob( std::shared_ptr< std::vector< Point_2 > > pointsPointer ) : pointsPointer( pointsPointer ) {
  std::cout << "TestCollinearityJob::TestCollinearityJob " << this << std::endl;
}

void
TestCollinearityJob::run( ThreadWeaver::JobPointer, ThreadWeaver::Thread* ) {
  std::cout << "TestCollinearityJob::run1 " << this << std::endl;

  sleep( 1 );

  for( std::size_t i = 0, end = pointsPointer->size(); i < ( end - 2 ); ++i ) {
    collinear &= CGAL::collinear( pointsPointer->at( i + 0 ), pointsPointer->at( i + 1 ), pointsPointer->at( i + 2 ) );

    if( !collinear ) {
      break;
    }
  }

  std::cout << "TestCollinearityJob::run2 " << this << std::endl;

  if( collinear ) {
    throw ThreadWeaver::JobFailed( "given points are collinear" );
  }
}
