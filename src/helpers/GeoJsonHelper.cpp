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

// https://gis.stackexchange.com/questions/64856/how-can-i-create-a-multipoint-geometry-in-qgis

// https://docs.qgis.org/3.16/en/docs/user_manual/processing_algs/qgis/vectorgeometry.html?highlight=setz#set-z-value

#include "GeoJsonHelper.h"

#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

GeoJsonHelper::GeoJsonHelper() = default;

GeoJsonHelper::GeoJsonHelper( QFile& file ) { parse( file ); }

GeoJsonHelper::~GeoJsonHelper() = default;

void
GeoJsonHelper::addFeature( const GeoJsonHelper::GeometryType& type, const GeoJsonHelper::FeatureType& feature ) {
  members.push_back( std::make_pair( type, feature ) );
}

void
GeoJsonHelper::parse( QFile& file ) {
  QByteArray saveData = file.readAll();

  QJsonDocument loadDoc( QJsonDocument::fromJson( saveData ) );

  auto json = loadDoc.object();
  parse( json );
}

std::optional< GeoJsonHelper::PointType >
GeoJsonHelper::parseCoodinate( const QJsonValue& jsonValue ) {
  if( jsonValue.isArray() ) {
    QJsonArray coordinate = jsonValue.toArray();

    if( coordinate.size() >= 2 ) {
      double height = 0;

      if( coordinate.size() >= 3 ) {
        height = coordinate.at( 2 ).toDouble();
      };

      return PointType( coordinate.at( 1 ).toDouble(), coordinate.at( 0 ).toDouble(), height );
    }
  }

  return std::nullopt;
}

QJsonArray
GeoJsonHelper::assembleCoordinatesArray( const GeoJsonHelper::PointVector& points ) {
  QJsonArray coordinates;

  for( const auto& point : points ) {
    coordinates.push_back( assembleCoodinate( point ) );
  }

  return coordinates;
}

QJsonArray
GeoJsonHelper::assembleCoodinate( const GeoJsonHelper::PointType& point ) {
  QJsonArray coordinate;

  coordinate.push_back( point.y() );
  coordinate.push_back( point.x() );
  if( point.z() != 0 ) {
    coordinate.push_back( point.z() );
  }

  return coordinate;
}

GeoJsonHelper::PointVector
GeoJsonHelper::parseCoordinatesArray( const QJsonArray& coordinatesArray ) {
  PointVector vector;

  for( const auto& blockIndex : qAsConst( coordinatesArray ) ) {
    if( auto parsedCoordinates = parseCoodinate( blockIndex ) ) {
      vector.push_back( std::move( parsedCoordinates.value() ) );
    }
  }

  return vector;
}

GeoJsonHelper::GeometryType
GeoJsonHelper::deduceType( const QJsonObject& object ) {
  if( auto type = getJsonQString( object, QStringLiteral( "type" ) ) ) {
    auto& typeString = type.value();

    if( typeString == QLatin1String( "Point" ) ) {
      return GeometryType::Point;
    }

    if( typeString == QLatin1String( "MultiPoint" ) ) {
      return GeometryType::MultiPoint;
    }

    if( typeString == QLatin1String( "LineString" ) ) {
      return GeometryType::LineString;
    }

    if( typeString == QLatin1String( "Polygon" ) ) {
      return GeometryType::Polygon;
    }

    if( typeString == QLatin1String( "MultiPolygon" ) ) {
      return GeometryType::MultiPolygon;
    }
  }

  return GeometryType::None;
}

std::optional< QJsonArray >
GeoJsonHelper::getJsonArray( const QJsonObject& object, const QString& name ) {
  if( object.contains( name ) && object[name].isArray() ) {
    return object[name].toArray();
  }

  return std::nullopt;
}

std::optional< QJsonObject >
GeoJsonHelper::getJsonObject( const QJsonObject& object, const QString& name ) {
  if( object.contains( name ) && object[name].isObject() ) {
    return object[name].toObject();
  }

  return std::nullopt;
}

std::optional< QString >
GeoJsonHelper::getJsonQString( const QJsonObject& object, const QString& name ) {
  if( object.contains( name ) && object[name].isString() ) {
    return object[name].toString();
  }

  return std::nullopt;
}

bool
GeoJsonHelper::testForFieldWithValue( const QJsonObject& object, const QString& name, const QString& value ) {
  if( auto field = getJsonQString( object, name ) ) {
    return field.value() == value;
  }

  return false;
}

void
GeoJsonHelper::parse( const QJsonObject& json ) {
  if( testForFieldWithValue( json, QStringLiteral( "type" ), QStringLiteral( "FeatureCollection" ) ) ) {
    if( auto featuresArray = getJsonArray( json, QStringLiteral( "features" ) ) ) {
      for( const auto& feature : qAsConst( featuresArray.value() ) ) {
        QJsonObject featuresObject = feature.toObject();

        if( testForFieldWithValue( featuresObject, QStringLiteral( "type" ), QStringLiteral( "Feature" ) ) ) {
          if( auto geometryObject = getJsonObject( featuresObject, QStringLiteral( "geometry" ) ) ) {
            auto type = deduceType( geometryObject.value() );

            switch( type ) {
              case GeometryType::Point: {
                if( auto coordinatesJsonArray = getJsonArray( geometryObject.value(), QStringLiteral( "coordinates" ) ) ) {
                  if( auto coordinate = parseCoodinate( coordinatesJsonArray.value() ) ) {
                    members.push_back( std::make_pair( type, coordinate.value() ) );
                  }
                }
              } break;

              case GeometryType::MultiPoint:
              case GeometryType::LineString: {
                if( auto coordinatesJsonArray = getJsonArray( geometryObject.value(), QStringLiteral( "coordinates" ) ) ) {
                  members.push_back( std::make_pair( type, parseCoordinatesArray( coordinatesJsonArray.value() ) ) );
                }

              } break;

              case GeometryType::Polygon: {
                if( auto coordinatesJsonArray = getJsonArray( geometryObject.value(), QStringLiteral( "coordinates" ) ) ) {
                  PointVectorVector polygon;

                  for( const auto& coordinatesArray : qAsConst( coordinatesJsonArray.value() ) ) {
                    if( coordinatesArray.isArray() ) {
                      polygon.push_back( parseCoordinatesArray( coordinatesArray.toArray() ) );
                    }
                  }

                  members.push_back( std::make_pair( type, polygon ) );
                }
              } break;

              case GeometryType::MultiPolygon: {
                if( auto coordinatesJsonArray = getJsonArray( geometryObject.value(), QStringLiteral( "coordinates" ) ) ) {
                  for( const auto& coordinatesArray : qAsConst( coordinatesJsonArray.value() ) ) {
                    PointVectorVector polygon;

                    if( coordinatesArray.isArray() ) {
                      for( const auto& coordinatesArrayNested : coordinatesArray.toArray() ) {
                        if( coordinatesArrayNested.isArray() ) {
                          polygon.push_back( parseCoordinatesArray( coordinatesArrayNested.toArray() ) );
                        }
                      }
                    }

                    members.push_back( std::make_pair( GeometryType::Polygon, polygon ) );
                  }
                }
              } break;

              default:
                break;
            }
          }
        }
      }
    }
  }
}

void
GeoJsonHelper::save( QFile& file ) {
  QJsonDocument jsonDocument( save() );
  file.write( jsonDocument.toJson() );
}

QJsonObject
GeoJsonHelper::save() {
  QJsonObject jsonObject;

  jsonObject[QStringLiteral( "type" )] = QStringLiteral( "FeatureCollection" );

  QJsonArray features;

  for( const auto& member : members ) {
    QJsonObject feature;

    feature[QStringLiteral( "type" )] = QStringLiteral( "Feature" );

    QJsonObject geometry;

    switch( member.first ) {
      case GeometryType::Point: {
        geometry[QStringLiteral( "type" )]        = QStringLiteral( "Point" );
        geometry[QStringLiteral( "coordinates" )] = assembleCoodinate( std::get< PointType >( member.second ) );
      } break;

      case GeometryType::MultiPoint: {
        geometry[QStringLiteral( "type" )]        = QStringLiteral( "MultiPoint" );
        geometry[QStringLiteral( "coordinates" )] = assembleCoordinatesArray( std::get< MultiPointType >( member.second ) );
      } break;

      case GeometryType::LineString: {
        geometry[QStringLiteral( "type" )]        = QStringLiteral( "LineString" );
        geometry[QStringLiteral( "coordinates" )] = assembleCoordinatesArray( std::get< LineStringType >( member.second ) );
      } break;

      case GeometryType::Polygon: {
        geometry[QStringLiteral( "type" )] = QStringLiteral( "Polygon" );
        QJsonArray coordinates;

        for( const auto& polygon : std::get< PolygonType >( member.second ) ) {
          coordinates.push_back( assembleCoordinatesArray( polygon ) );
        }

        geometry[QStringLiteral( "coordinates" )] = coordinates;
      } break;

      default:
        break;
    }

    feature[QStringLiteral( "geometry" )] = geometry;
    features.push_back( feature );
  }

  jsonObject[QStringLiteral( "features" )] = features;

  return jsonObject;
}

void
GeoJsonHelper::print() {
  using namespace std;
  Eigen::IOFormat CommaInitFmt( Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "(", ")" );
  cout << "GeoJsonHelper, n: " << members.size() << endl;

  for( const auto& member : members ) {
    switch( member.first ) {
      case GeometryType::Point:
        cout << "GeometryType::Point: " << std::get< PointType >( member.second ).format( CommaInitFmt ) << endl;
        break;

      case GeometryType::MultiPoint: {
        auto points = std::get< PointVector >( member.second );
        cout << "  GeometryType::MultiPoint(" << points.size() << "): ";

        for( const auto& point : points ) {
          cout << point.format( CommaInitFmt ) << " ";
        }

        cout << endl;

      } break;

      case GeometryType::LineString: {
        auto points = std::get< PointVector >( member.second );
        cout << "  GeometryType::LineString(" << points.size() << "): ";

        for( const auto& point : points ) {
          cout << point.format( CommaInitFmt ) << " ";
        }

        cout << endl;

      } break;

      case GeometryType::Polygon: {
        auto polygons = std::get< PointVectorVector >( member.second );
        cout << "  GeometryType::Polygon(" << polygons.size() << ")";

        for( const auto& vector : polygons ) {
          cout << "    Polygon(" << vector.size() << "): ";

          for( const auto& point : vector ) {
            cout << point.format( CommaInitFmt ) << " ";
          }

          cout << endl;
        }
      } break;

      default:
        break;
    }
  }
}
