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
#include "PathPrimitiveRay.h"
#include "PathPrimitiveLine.h"
#include "PathPrimitiveSegment.h"

#include <QDebug>

void PathPrimitiveSequence::updateWithPolyline( const std::vector<Point_2>& polyline ) {
  this->polyline.clear();
  std::copy( polyline.cbegin(), polyline.cend(), std::back_inserter( this->polyline ) );

  supportLine = Line_2( polyline.front(), polyline.back() );

  bisectors.clear();

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

      for( int i = 1, end = polyline.size() - 2; i != end; ++i ) {
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

  for( size_t i = 0, end = sequence.size() - 1; i < end; ++i ) {
    bisectors.push_back(
      CGAL::bisector(
        sequence.at( i )->supportingLine().opposite(),
        sequence.at( i + 1 )->supportingLine() ) );

    if( !bisectors.back().has_on_negative_side( polyline.at( i ) ) ) {
      bisectors.back() = bisectors.back().opposite();
    }
  }
}

std::shared_ptr<PathPrimitive> PathPrimitiveSequence::createReverse() {
  std::vector<std::shared_ptr<PathPrimitive>> sequenceNew;
  std::vector<Line_2> bisectorsNew;

  for( auto it = sequence.crbegin(), end = sequence.crend(); it != end; ++it ) {
    sequenceNew.push_back( ( *it )->createReverse() );
  }

  for( auto it = bisectors.crbegin(), end = bisectors.crend(); it != end; ++it ) {
    bisectorsNew.push_back( it->opposite() );
  }

  return std::make_shared<PathPrimitiveSequence>( sequenceNew, bisectorsNew, implementWidth, anyDirection, passNumber );
}

std::shared_ptr<PathPrimitive> PathPrimitiveSequence::createNextPrimitive( bool left, bool reverse ) {
  int passNumberNew = passNumber + ( left ? 1 : -1 );

  if( sequence.size() == 1 ) {
    return std::make_shared<PathPrimitiveLine>( sequence.front()->supportingLine(), implementWidth, anyDirection, passNumberNew );
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
  std::copy( bisectors.cbegin(), bisectors.cend(), std::back_inserter( bisectorsNew ) );

  for( auto it = sequence.cbegin(), end = sequence.cend(); it != end; ++it ) {
    primitives.push_back( ( *it )->createNextPrimitive( left, reverse ) );
  }

  if( bisectorsNew.size() >= 2 ) {
    double implementWidthSquared = implementWidth * implementWidth;

    for( size_t i = 0; i < bisectorsNew.size() - 1; ) {
      auto result = CGAL::intersection( bisectorsNew.at( i ), bisectorsNew.at( i + 1 ) );

      if( result ) {
        if( const Point_2* point = boost::get<Point_2>( &*result ) ) {
          // both on the same side?
          /*if( !( left ^ primitives.at( i + 1 )->leftOf( *point ) ) )*/ {
            double distanceSquared = //primitives.at( i + 1 )->distanceToPointSquared(*point);
              CGAL::squared_distance( primitives.at( i + 1 )->supportingLine(), *point );

            if( distanceSquared < implementWidthSquared ) {
              if( ( i < ( primitives.size() - 1 ) ) && i != 0 ) {
                Point_2 source, target;

                switch( primitives.at( i )->getType() ) {
                  case PathPrimitive::Type::Segment:
                    source = primitives.at( i )->castToSegment()->segment.source();
                    break;

                  case PathPrimitive::Type::Ray:
                    source = primitives.at( i )->castToRay()->ray.source();
                    break;

                  default:
                    break;
                }

                switch( primitives.at( i + 1 )->getType() ) {
                  case PathPrimitive::Type::Segment:
                    target = primitives.at( i + 1 )->castToSegment()->segment.target();
                    break;

                  case PathPrimitive::Type::Ray:
                    target = primitives.at( i + 1 )->castToRay()->ray.source();
                    break;

                  default:
                    break;
                }

                primitives.insert( primitives.cbegin() + i, std::static_pointer_cast<PathPrimitive>(
                                     std::make_shared<PathPrimitiveSegment> (
                                       Segment_2( source, target ),
                                       implementWidth, anyDirection, passNumberNew ) ) );
                primitives.erase( primitives.cbegin() + i + 1, primitives.cbegin() + i + 3 );

                bisectorsNew.erase( bisectorsNew.cbegin() + i );

                bisectorsNew.at( i - 1 ) = CGAL::bisector(
                                             primitives.at( i - 1 )->supportingLine().opposite(),
                                             primitives.at( i )->supportingLine() );

                if( primitives.at( i - 1 )->supportingLine().has_on_positive_side( bisectorsNew.at( i - 1 ).point( 1 ) ) ) {
                  bisectorsNew.at( i - 1 ) = bisectorsNew.at( i - 1 ).opposite();
                }

                bisectorsNew.at( i ) = CGAL::bisector(
                                         primitives.at( i )->supportingLine().opposite(),
                                         primitives.at( i + 1 )->supportingLine() );

                if( primitives.at( i )->supportingLine().has_on_positive_side( bisectorsNew.at( i ).point( 1 ) ) ) {
                  bisectorsNew.at( i ) = bisectorsNew.at( i ).opposite();
                }


              } else {
                primitives.erase( primitives.cbegin() + i + 1 );
                bisectorsNew.erase( bisectorsNew.cbegin() + i );

                bisectorsNew.at( i ) = CGAL::bisector(
                                         primitives.at( i )->supportingLine().opposite(),
                                         primitives.at( i + 1 )->supportingLine() );

                if( primitives.at( i )->supportingLine().has_on_positive_side( bisectorsNew.at( i ).point( 1 ) ) ) {
                  bisectorsNew.at( i ) = bisectorsNew.at( i ).opposite();
                }
              }

              // restart the algorythm
              i = 0;
              continue;
            }
          }
        }
      }

      ++i;
    }
  }

  // create the new sequence
  if( primitives.size() == 1 ) {
    return std::make_shared<PathPrimitiveLine>( primitives.front()->supportingLine(), implementWidth, anyDirection, passNumberNew );
  }

  if( primitives.size() == 2 ) {
    auto result = CGAL::intersection( primitives.front()->supportingLine(), primitives.back()->supportingLine() );

    if( result ) {
      if( const Point_2* point = boost::get<Point_2>( &*result ) ) {
        std::vector<std::shared_ptr<PathPrimitive>> sequenceNew;
        sequenceNew.push_back( std::static_pointer_cast<PathPrimitive>(
                                 std::make_shared<PathPrimitiveRay>(
                                   Ray_2( *point, primitives.front()->castToRay()->ray.direction() ),
                                   true, implementWidth, anyDirection, passNumberNew ) ) );
        sequenceNew.push_back( std::static_pointer_cast<PathPrimitive>(
                                 std::make_shared<PathPrimitiveRay> (
                                   Ray_2( *point, primitives.back()->castToRay()->ray.direction() ),
                                   false, implementWidth, anyDirection, passNumberNew ) ) );
        return std::make_shared<PathPrimitiveSequence>( sequenceNew, bisectorsNew, implementWidth, anyDirection, passNumberNew );
      }
    } else {
      return std::make_shared<PathPrimitiveLine>( primitives.front()->supportingLine(), implementWidth, anyDirection, passNumberNew );
    }
  }

  {
    std::vector<std::shared_ptr<PathPrimitive>> sequenceNew;
    Point_2 lastIntersection = Point_2( 0, 0 );

    for( size_t i = 0, end = primitives.size(); i < end; ++i ) {
      auto primitiveRay = primitives.at( i )->castToRay();
      auto primitiveSegment = primitives.at( i )->castToSegment();


      // first ray
      if( primitiveRay ) {
        if( primitiveRay->reverse ) {
          auto result = CGAL::intersection( primitives.at( i )->supportingLine(), bisectorsNew.at( i ) );

          if( result ) {
            if( const Point_2* point = boost::get<Point_2>( &*result ) ) {
              sequenceNew.push_back( std::static_pointer_cast<PathPrimitive>(
                                       std::make_shared<PathPrimitiveRay> (
                                         Ray_2( *point, primitiveRay->ray.direction() ),
                                         true, implementWidth, anyDirection, passNumberNew ) ) );
              lastIntersection = *point;
              continue;
            }
          }

          // second ray
        } else {
          auto result = CGAL::intersection( primitives.at( i )->supportingLine(), bisectorsNew.at( i - 1 ) );

          if( result ) {
            if( const Point_2* point = boost::get<Point_2>( &*result ) ) {
              sequenceNew.push_back( std::static_pointer_cast<PathPrimitive>(
                                       std::make_shared<PathPrimitiveRay> (
                                         Ray_2( *point, primitiveRay->ray.direction() ),
                                         false, implementWidth, anyDirection, passNumberNew ) ) );
              lastIntersection = *point;
              continue;
            }
          }
        }
      }

      if( primitiveSegment ) {
        auto result = CGAL::intersection( primitives.at( i )->supportingLine(), bisectorsNew.at( i ) );
        auto result2 = CGAL::intersection( primitives.at( i )->supportingLine(), bisectorsNew.at( i - 1 ) );

        if( result ) {
          if( const Point_2* point = boost::get<Point_2>( &*result ) ) {
            if( const Point_2* point2 = boost::get<Point_2>( &*result2 ) ) {
              sequenceNew.push_back( std::static_pointer_cast<PathPrimitive>(
                                       std::make_shared<PathPrimitiveSegment> (
                                         Segment_2( *point2, *point ),
                                         implementWidth, anyDirection, passNumberNew ) ) );
              lastIntersection = *point;
              continue;
            }
          }
        }
      }
    }

    return std::make_shared<PathPrimitiveSequence>( sequenceNew, bisectorsNew, implementWidth, anyDirection, passNumberNew );
  }
}

std::shared_ptr<PathPrimitive>& PathPrimitiveSequence::findSequencePrimitive( const Point_2& point ) {
  if( bisectors.empty() ) {
    return sequence.front();
  } else {
    int foundBisector = -1;

    for( size_t i = 0, end = bisectors.size(); i < end; ++i ) {
      if( bisectors.at( i ).has_on_positive_side( point ) ) {
        foundBisector = i;
      } else {
        break;
      }
    }

    return sequence.at( foundBisector + 1 );
  }
}

double PathPrimitiveSequence::distanceToPointSquared( const Point_2& point ) {
  if( sequence.empty() ) {
    return 0;
  }

  return findSequencePrimitive( point )->distanceToPointSquared( point );
}

bool PathPrimitiveSequence::isOn( const Point_2& /*point*/ ) {
  if( sequence.empty() ) {
    return false;
  }

  return true;
}

bool PathPrimitiveSequence::leftOf( const Point_2& point ) {
  if( sequence.empty() ) {
    return false;
  }

  return findSequencePrimitive( point )->leftOf( point );
}

double PathPrimitiveSequence::angleAtPointDegrees( const Point_2& point ) {
  if( sequence.empty() ) {
    return 0;
  }

  return findSequencePrimitive( point )->angleAtPointDegrees( point );
}

bool PathPrimitiveSequence::intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) {
  for( auto it = sequence.cbegin(), end = sequence.cend(); it != end; ++it ) {
    if( ( *it )->intersectWithLine( lineToIntersect, resultingPoint ) ) {
      return true;
    }
  }

  return false;
}

Line_2 PathPrimitiveSequence::perpendicularAtPoint( const Point_2 point ) {
  if( sequence.empty() ) {
    return Line_2();
  }

  return findSequencePrimitive( point )->perpendicularAtPoint( point );
}

Point_2 PathPrimitiveSequence::orthogonalProjection( const Point_2 point ) {
  if( sequence.empty() ) {
    return Point_2();
  }

  return findSequencePrimitive( point )->orthogonalProjection( point );
}

Line_2& PathPrimitiveSequence::supportingLine() {
  return supportLine;
}

void PathPrimitiveSequence::transform( const Aff_transformation_2& transformation ) {
  for( auto it = sequence.cbegin(), end = sequence.cend(); it != end; ++it ) {
    ( *it )->transform( transformation );
  }
}
