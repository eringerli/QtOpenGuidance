// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
