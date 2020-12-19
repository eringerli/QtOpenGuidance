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

#include "PathPrimitiveSequence.h"
#include "PathPrimitiveLine.h"
#include "PathPrimitiveRay.h"
#include "PathPrimitiveSegment.h"

#include <QDebug>

void PathPrimitiveSequence::createBisectors( std::back_insert_iterator<std::vector<Line_2>> bisectorsOutputIterator, const std::vector<std::shared_ptr<PathPrimitive>>& primitives ) {
  for( size_t i = 0, end = primitives.size() - 1; i < end; ++i ) {
    *bisectorsOutputIterator++ = CGAL::bisector(
                                         sequence.at( i )->supportingLine( Point_2() ).opposite(),
                                         sequence.at( i + 1 )->supportingLine( Point_2() ) );
  }
}

void PathPrimitiveSequence::updateWithPolyline( const std::vector<Point_2>& polyline ) {
  this->polyline.clear();
  std::copy( polyline.cbegin(), polyline.cend(), std::back_inserter( this->polyline ) );

  supportLine = Line_2( polyline.front(), polyline.back() );


  if( polyline.size() == 2 ) {
    sequence.clear();
    sequence.push_back( std::static_pointer_cast<PathPrimitive>(
                                std::make_shared<PathPrimitiveLine> (
                                        Line_2( polyline.at( 0 ), polyline.at( 1 ) ),
                                        implementWidth, anyDirection, passNumber ) ) );
    return;
  }

  if( polyline.size() == 3 ) {
    sequence.clear();
    sequence.push_back( std::static_pointer_cast<PathPrimitive>(
                                std::make_shared<PathPrimitiveRay>(
                                        Ray_2( polyline.at( 1 ), polyline.at( 0 ) ),
                                        true, implementWidth, anyDirection, passNumber ) ) );
    sequence.push_back( std::static_pointer_cast<PathPrimitive>(
                                std::make_shared<PathPrimitiveRay> (
                                        Ray_2( polyline.at( 1 ), polyline.at( 2 ) ),
                                        false, implementWidth, anyDirection, passNumber ) ) );
  } else {
    if( polyline.size() > 3 ) {
      sequence.clear();

      sequence.push_back( std::static_pointer_cast<PathPrimitive>(
                                  std::make_shared<PathPrimitiveRay>(
                                          Ray_2( polyline.at( 1 ), polyline.at( 0 ) ),
                                          true, implementWidth, anyDirection, passNumber ) ) );

      for( int i = 1, end = polyline.size() - 2; i < end; ++i ) {
        sequence.push_back( std::static_pointer_cast<PathPrimitive>(
                                    std::make_shared<PathPrimitiveSegment> (
                                            Segment_2( polyline.at( i ), polyline.at( i + 1 ) ),
                                            implementWidth, anyDirection, passNumber ) ) );
      }

      sequence.push_back( std::static_pointer_cast<PathPrimitive>(
                                  std::make_shared<PathPrimitiveRay> (
                                          Ray_2( polyline.at( polyline.size() - 2 ), polyline.at( polyline.size() - 1 ) ),
                                          false, implementWidth, anyDirection, passNumber ) ) );
    }
  }

  bisectors.clear();
  createBisectors( std::back_inserter( bisectors ), sequence );

  orderBisectors( bisectors, sequence );
}

std::shared_ptr<PathPrimitive> PathPrimitiveSequence::createReverse() {
  std::vector<std::shared_ptr<PathPrimitive>> sequenceNew;
  std::vector<Line_2> bisectorsNew;

  for( const auto& it : sequence ) {
    sequenceNew.push_back( it->createReverse() );
  }

  createBisectors( std::back_inserter( bisectorsNew ), sequenceNew );

  orderBisectors( bisectorsNew, sequenceNew );

  return std::make_shared<PathPrimitiveSequence>( sequenceNew, bisectorsNew, implementWidth, anyDirection, passNumber );
}

std::shared_ptr<PathPrimitive> PathPrimitiveSequence::createNextPrimitive( bool left ) {
  int passNumberNew = passNumber + ( left ? 1 : -1 );

  if( sequence.size() == 1 ) {
    return std::make_shared<PathPrimitiveLine>( sequence.front()->supportingLine( Point_2() ), implementWidth, anyDirection, passNumberNew );
  }

  // This removes all segments in the list which would get artifacts if stretched/squashed.
  //
  // First, we move all the primitives one implement further (by calling createNextPrimitive() on them),
  // then we construct a straight skeleton (not a complete one, just bisectors between adjacent primitives).
  // Then we remove all segments which the bisectors on each side intersect within the implement width and
  // create/adjust the bisectors. Then we intersect the primitives with this skeleton to get the new points.
  //
  // Some magic happens to keep the data valid and the indices in range; the rays are not removed in any case
  // and the algorythm needs a ray on either side of the primitives-vector. Also the bisectors get saved and
  // oriented to accelerate the lookup of the nearest primitive.
  //
  // This gets valid results for reasonable inputs.

  std::vector<std::shared_ptr<PathPrimitive>> primitives;
  std::vector<Line_2> bisectorsNew;
//  std::copy( bisectors.cbegin(), bisectors.cend(), std::back_inserter( bisectorsNew ) );

  for( const auto& it : sequence ) {
    primitives.push_back( it->createNextPrimitive( left ) );
  }

  createBisectors( std::back_inserter( bisectorsNew ), primitives );

  if( bisectorsNew.size() >= 2 ) {
    double implementWidthSquared = implementWidth * implementWidth;

    for( size_t i = 0; i < bisectorsNew.size() - 1; ) {
      auto result = CGAL::intersection( bisectorsNew.at( i ), bisectorsNew.at( i + 1 ) );

      if( result ) {
        if( const Point_2* point = boost::get<Point_2>( &*result ) ) {
          if( left != ( primitives.at( i + 1 )->leftOf( *point ) ) ) {
            double distanceSquared = CGAL::squared_distance( primitives.at( i + 1 )->supportingLine( Point_2() ), *point );

            if( distanceSquared < implementWidthSquared ) {
              primitives.erase( primitives.cbegin() + i + 1 );

              bisectorsNew.at( i ) = CGAL::bisector(
                                             primitives.at( i )->supportingLine( Point_2() ).opposite(),
                                             primitives.at( i + 1 )->supportingLine( Point_2() ) );
              bisectorsNew.erase( bisectorsNew.cbegin() + i + 1 );

              // start again
              i = 0;
              continue;
            }
          }
        }
      }

      ++i;
    }
  }

  orderBisectors( bisectorsNew, primitives );

  // extend the primitives
  for( size_t i = 0; i < primitives.size() - 1; ++i ) {
    auto result2 = CGAL::intersection( primitives.at( i )->supportingLine( Point_2() ), primitives.at( i + 1 )->supportingLine( Point_2() ) );

    if( result2 ) {
      if( const Point_2* point2 = boost::get<Point_2>( &*result2 ) ) {
        if( primitives.at( i )->getType() == PathPrimitive::Type::Ray ) {
          primitives.at( i )->setSource( *point2 );
        } else {
          primitives.at( i )->setTarget( *point2 );
        }

        primitives.at( i + 1 )->setSource( *point2 );
      }
    }
  }

  // create the new sequence
  {
    if( primitives.size() == 1 ) {
      return std::make_shared<PathPrimitiveLine>( primitives.front()->supportingLine( Point_2() ), implementWidth, anyDirection, passNumberNew );
    }

    return std::make_shared<PathPrimitiveSequence>( primitives, bisectorsNew, implementWidth, anyDirection, passNumberNew );
  }
}

void PathPrimitiveSequence::orderBisectors( std::vector<Line_2>& bisectorsToOrder, const std::vector<std::shared_ptr<PathPrimitive> >& primitives ) {
  for( size_t i = 0; i < bisectorsToOrder.size(); ++i ) {
    Point_2 pointToTest;

    if( const auto* ray = primitives.at( i )->castToRay() ) {
      pointToTest = ray->ray.point( 1 );
    }

    if( const auto* segment = primitives.at( i )->castToSegment() ) {
      pointToTest = CGAL::midpoint( segment->segment.source(), segment->segment.target() );
    }

    if( bisectorsToOrder.at( i ).has_on_positive_side( pointToTest ) ) {
      bisectorsToOrder.at( i ) = bisectorsToOrder.at( i ).opposite();
    }
  }
}

const std::shared_ptr<PathPrimitive>& PathPrimitiveSequence::findSequencePrimitive( Point_2 point ) const {
  if( sequence.size() < 2 ) {
    return sequence.front();
  }

  if( bisectors.front().has_on_negative_side( point ) ) {
    return sequence.front();
  }

  if( sequence.size() > 1 ) {
    for( size_t i = 1, end = sequence.size() - 1; i < end; ++i ) {
      if( bisectors.at( i - 1 ).has_on_positive_side( point ) && bisectors.at( i ).has_on_negative_side( point ) ) {
        return sequence.at( i );
      }
    }
  }

  return sequence.back();
}

double PathPrimitiveSequence::distanceToPointSquared( const Point_2 point ) {
  if( sequence.empty() ) {
    return 0;
  }

  return findSequencePrimitive( point )->distanceToPointSquared( point );
}

bool PathPrimitiveSequence::isOn( const Point_2 /*point*/ ) {
  return !sequence.empty();
}

bool PathPrimitiveSequence::leftOf( const Point_2 point ) {
  if( sequence.empty() ) {
    return false;
  }

  return findSequencePrimitive( point )->leftOf( point );
}

double PathPrimitiveSequence::angleAtPointDegrees( const Point_2 point ) {
  if( sequence.empty() ) {
    return 0;
  }

  return findSequencePrimitive( point )->angleAtPointDegrees( point );
}

bool PathPrimitiveSequence::intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) {
  for( const auto& it : sequence ) {
    if( it->intersectWithLine( lineToIntersect, resultingPoint ) ) {
      return true;
    }
  }

  return false;
}

Line_2 PathPrimitiveSequence::perpendicularAtPoint( const Point_2 point ) {
  if( sequence.empty() ) {
    return {};
  }

  return findSequencePrimitive( point )->perpendicularAtPoint( point );
}

Point_2 PathPrimitiveSequence::orthogonalProjection( const Point_2 point ) {
  if( sequence.empty() ) {
    return {};
  }

  return findSequencePrimitive( point )->orthogonalProjection( point );
}

Line_2& PathPrimitiveSequence::supportingLine( const Point_2 point ) {
  if( sequence.empty() ) {
    return supportLine;
  }

  return findSequencePrimitive( point )->supportingLine( point );
}

void PathPrimitiveSequence::transform( const Aff_transformation_2& transformation ) {
  for( const auto& it : sequence ) {
    it->transform( transformation );
  }
}
