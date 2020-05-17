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

#pragma once

#include <QObject>

#include "BlockBase.h"

#include "UBX_Parser.h"

class UBX_Parser_Helper : public QObject, public UBX_Parser {
    Q_OBJECT

  public:
    UBX_Parser_Helper() {}
    ~UBX_Parser_Helper() {}

  protected:
    void handle_NAV_HPPOSLLH(
            uint32_t iTOW,
            double lon,
            double lat,
            double height,
            double hMSL,
            double hAcc,
            double vAcc ) override {
      emit ubxNavHpPosLlhChanged( iTOW, lon, lat, height, hMSL, hAcc, vAcc );
    }

    void handle_NAV_RELPOSNED(
            uint32_t iTOW,
            double relPosN,
            double relPosE,
            double relPosD,
            double relPosLenght,
            double relPosHeading ) override {
      emit ubxNavRelPosNedChanged( iTOW, relPosN, relPosE, relPosD, relPosLenght, relPosHeading );
    }

    void handle_NAV_PVT(
            uint32_t iTOW,
            bool rtkFix,
            bool rtkFloat,
            uint8_t numSatelites,
            double speed,
            double headingOfMotion,
            double headingOfVehicle,
            double pDOP ) override {
      emit UbxNavPvtChanged( iTOW, rtkFix, rtkFloat, numSatelites, speed, headingOfMotion, headingOfVehicle, pDOP );
    }

  signals:
    void ubxNavHpPosLlhChanged(
            uint32_t iTOW,
            double lon,
            double lat,
            double height,
            double hMSL,
            double hAcc,
            double vAcc );

    void ubxNavRelPosNedChanged(
            uint32_t iTOW,
            double relPosN,
            double relPosE,
            double relPosD,
            double relPosLenght,
            double relPosHeading );

    void UbxNavPvtChanged(
            uint32_t iTOW,
            bool rtkFix,
            bool rtkFloat,
            uint8_t numSatelites,
            double speed,
            double headingOfMotion,
            double headingOfVehicle,
            double pDOP );
};

class UbxParser : public BlockBase {
    Q_OBJECT

  public:
    explicit UbxParser()
      : BlockBase() {
      connect( &ubxParser, &UBX_Parser_Helper::ubxNavHpPosLlhChanged, this, &UbxParser::ubxNavHpPosLLH );
      connect( &ubxParser, &UBX_Parser_Helper::ubxNavRelPosNedChanged, this, &UbxParser::ubxNavRelPosNed );
      connect( &ubxParser, &UBX_Parser_Helper::UbxNavPvtChanged, this, &UbxParser::UbxNavPvt );
    }

  signals:
    void globalPositionChanged( const double, const double, const double );
    void velocityChanged( const double );

    void orientationDualAntennaChanged( const QQuaternion& );
    void orientationVehicleChanged( const QQuaternion& );
    void orientationMotionChanged( const QQuaternion& );

    void distanceBetweenAntennasChanged( const double );
    void numSatelitesChanged( const double );
    void hdopChanged( const double );
    void horizontalAccuracyChanged( const double );
    void verticalAccuracyChanged( const double );

  public slots:
    void setData( const QByteArray& data ) {
      for( const uint8_t c : data ) {
        ubxParser.parse( c );
      }
    }

  protected slots:
    void ubxNavHpPosLLH(
            uint32_t /*iTOW*/,
            double lon,
            double lat,
            double height,
            double /*hMSL*/,
            double hAcc,
            double vAcc ) {
      emit globalPositionChanged( lon, lat, height );
      emit horizontalAccuracyChanged( hAcc );
      emit verticalAccuracyChanged( vAcc );
    }

    void ubxNavRelPosNed(
            uint32_t /*iTOW*/,
            double /*relPosN*/,
            double /*relPosE*/,
            double relPosD,
            double relPosLenght,
            double relPosHeading ) {
      emit orientationDualAntennaChanged(
              // roll
              QQuaternion::fromAxisAndAngle(
                      QVector3D( 1.0f, 0.0f, 0.0f ),
                      float( ( std::asin( relPosD / relPosLenght ) - rollOffset )*rollFactor ) )
              *
              // heading
              QQuaternion::fromAxisAndAngle(
                      QVector3D( 0.0f, 0.0f, 1.0f ),
                      float( ( relPosHeading - rollOffset )*rollFactor ) )
      );
      emit distanceBetweenAntennasChanged( relPosLenght );
    }

    void UbxNavPvt(
            uint32_t /*iTOW*/,
            bool /*rtkFix*/,
            bool /*rtkFloat*/,
            uint8_t numSatelites,
            double speed,
            double headingOfMotion,
            double headingOfVehicle,
            double pDOP ) {

      emit velocityChanged( speed );
      emit numSatelitesChanged( numSatelites );
      emit hdopChanged( pDOP );

      emit orientationMotionChanged( QQuaternion::fromAxisAndAngle(
                                             QVector3D( 0.0f, 0.0f, 1.0f ),
                                             float( ( headingOfMotion - headingOffset )*headingFactor ) ) );

      emit orientationVehicleChanged( QQuaternion::fromAxisAndAngle(
                                              QVector3D( 0.0f, 0.0f, 1.0f ),
                                              float( ( headingOfVehicle - headingOffset )*headingFactor ) ) );
    }

    void setHeadingOffset( const double headingOffset ) {
      this->headingOffset = headingOffset;
    }
    void setHeadingFactor( const double headingFactor ) {
      this->headingFactor = headingFactor;
    }

    void setRollOffset( const double rollOffset ) {
      this->rollOffset = rollOffset;
    }
    void setRollFactor( const double rollFactor ) {
      this->rollFactor = rollFactor;
    }

  private:
    UBX_Parser_Helper ubxParser;
    double headingOffset = 0;
    double rollOffset = 0;
    double headingFactor = 0;
    double rollFactor = 0;
};

class UbxParserFactory : public BlockFactory {
    Q_OBJECT

  public:
    UbxParserFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "UBX Parser" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new UbxParser();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ) );
      b->addInputPort( QStringLiteral( "Heading Offset" ), QLatin1String( SLOT( setHeadingOffset( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Heading Factor" ), QLatin1String( SLOT( setHeadingFactor( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Roll Offset" ), QLatin1String( SLOT( setRollOffset( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Roll Factor" ), QLatin1String( SLOT( setRollFactor( const double ) ) ) );

      b->addOutputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SIGNAL( globalPositionChanged( const double, const double, const double ) ) ) );
      b->addOutputPort( QStringLiteral( "Velocity" ), QLatin1String( SIGNAL( velocityChanged( const double ) ) ) );
      b->addOutputPort( QStringLiteral( "Orientation Dual Antenna" ), QLatin1String( SIGNAL( orientationDualAntennaChanged( const QQuaternion& ) ) ) );
      b->addOutputPort( QStringLiteral( "Orientation GNS/Vehicle" ), QLatin1String( SIGNAL( orientationVehicleChanged( const QQuaternion& ) ) ) );
      b->addOutputPort( QStringLiteral( "Orientation GNS/Motion" ), QLatin1String( SIGNAL( orientationMotionChanged( const QQuaternion& ) ) ) );

      b->addOutputPort( QStringLiteral( "Dist Antennas" ), QLatin1String( SIGNAL( distanceBetweenAntennasChanged( const double ) ) ) );
      b->addOutputPort( QStringLiteral( "Num Satelites" ), QLatin1String( SIGNAL( numSatelitesChanged( const double ) ) ) );
      b->addOutputPort( QStringLiteral( "HDOP" ), QLatin1String( SIGNAL( hdopChanged( const double ) ) ) );
      b->addOutputPort( QStringLiteral( "Horizontal Accuracy" ), QLatin1String( SIGNAL( horizontalAccuracyChanged( const double ) ) ) );
      b->addOutputPort( QStringLiteral( "Vertical Accuracy" ), QLatin1String( SIGNAL( verticalAccuracyChanged( const double ) ) ) );

      b->setBrush( parserColor );

      return b;
    }
};
