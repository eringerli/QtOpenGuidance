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

#pragma once

#include <QObject>

#include <cstdint>
#include <memory>
#include <variant>
#include <utility>
#include <optional>

class QFile;
class QJsonObject;

#include "eigenHelper.h"

class GeoJsonHelper {
  public:
    GeoJsonHelper();
    GeoJsonHelper( QFile& file );
    ~GeoJsonHelper();

    enum class GeometryType : uint8_t {
      None,
      Point,
      MultiPoint,
      LineString,
      Polygon,
      MultiPolygon
    };

    using PointType = Eigen::Vector3d;
    using PointVector = std::vector<PointType>;
    using PointVectorVector = std::vector<std::vector<PointType>>;


    using MultiPointType = PointVector;
    using LineStringType = PointVector;
    using PolygonType = PointVectorVector;
    using MultiPolygonType = PointVectorVector;

    using FeatureType = std::variant<PointType, PointVector, PointVectorVector>;

    void addFeature( const GeometryType& type, const FeatureType& feature );

    void parse( QFile& file );
    void parse( const QJsonObject& json );

    void save( QFile& file );
    QJsonObject save();

    void print();

  public:
    std::vector<std::pair<GeometryType, FeatureType>> members;

  private:
    std::optional<QJsonArray> getJsonArray( const QJsonObject& object, const QString& name );
    std::optional<QJsonObject> getJsonObject( const QJsonObject& object, const QString& name );
    std::optional<QString> getJsonQString( const QJsonObject& object, const QString& name );

    bool testForFieldWithValue( const QJsonObject& object, const QString& name, const QString& value );

    PointVector parseCoordinatesArray( const QJsonArray& coordinatesArray );
    std::optional<PointType> parseCoodinate( const QJsonValue& jsonValue );

    QJsonArray assembleCoordinatesArray( const PointVector& points );
    QJsonArray assembleCoodinate( const PointType& point );

    GeometryType deduceType( const QJsonObject& object );
};
