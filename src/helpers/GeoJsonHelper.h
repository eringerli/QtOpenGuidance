// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

// https://gis.stackexchange.com/questions/64856/how-can-i-create-a-multipoint-geometry-in-qgis
// https://docs.qgis.org/3.16/en/docs/user_manual/processing_algs/qgis/vectorgeometry.html?highlight=setz#set-z-value

#pragma once

#include <QObject>

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <variant>

class QFile;
class QJsonObject;

#include "eigenHelper.h"

class GeoJsonHelper {
public:
  GeoJsonHelper();
  GeoJsonHelper( QFile& file );
  ~GeoJsonHelper();

  enum class GeometryType : uint8_t { None, Point, MultiPoint, LineString, Polygon, MultiPolygon };

  using PointType         = Eigen::Vector3d;
  using PointVector       = std::vector< PointType >;
  using PointVectorVector = std::vector< std::vector< PointType > >;

  using MultiPointType   = PointVector;
  using LineStringType   = PointVector;
  using PolygonType      = PointVectorVector;
  using MultiPolygonType = PointVectorVector;

  using FeatureType = std::variant< PointType, PointVector, PointVectorVector >;

  void addFeature( const GeometryType& type, const FeatureType& feature );

  void parse( QFile& file );
  void parse( const QJsonObject& json );

  void        save( QFile& file );
  QJsonObject save();

  void print();

public:
  std::vector< std::pair< GeometryType, FeatureType > > members;

private:
  std::optional< QJsonArray >  getJsonArray( const QJsonObject& object, const QString& name );
  std::optional< QJsonObject > getJsonObject( const QJsonObject& object, const QString& name );
  std::optional< QString >     getJsonQString( const QJsonObject& object, const QString& name );

  bool testForFieldWithValue( const QJsonObject& object, const QString& name, const QString& value );

  PointVector                parseCoordinatesArray( const QJsonArray& coordinatesArray );
  std::optional< PointType > parseCoodinate( const QJsonValue& jsonValue );

  QJsonArray assembleCoordinatesArray( const PointVector& points );
  QJsonArray assembleCoodinate( const PointType& point );

  GeometryType deduceType( const QJsonObject& object );
};
