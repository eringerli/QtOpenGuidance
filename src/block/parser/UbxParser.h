// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "UBX_Parser.h"

#include "helpers/eigenHelper.h"

class UBX_Parser_Helper
    : public QObject
    , public UBX_Parser {
  Q_OBJECT

public:
  UBX_Parser_Helper() {}
  ~UBX_Parser_Helper() {}

protected:
  void handle_NAV_HPPOSLLH( uint32_t iTOW, double lon, double lat, double height, double hMSL, double hAcc, double vAcc ) override;

  void
  handle_NAV_RELPOSNED( uint32_t iTOW, double relPosN, double relPosE, double relPosD, double relPosLenght, double relPosHeading ) override;

  void handle_NAV_PVT( uint32_t iTOW,
                       bool     rtkFix,
                       bool     rtkFloat,
                       uint8_t  numSatelites,
                       double   speed,
                       double   headingOfMotion,
                       double   headingOfVehicle,
                       double   pDOP ) override;

Q_SIGNALS:
  void ubxNavHpPosLlhChanged( uint32_t iTOW, double lon, double lat, double height, double hMSL, double hAcc, double vAcc );

  void ubxNavRelPosNedChanged( uint32_t iTOW, double relPosN, double relPosE, double relPosD, double relPosLenght, double relPosHeading );

  void UbxNavPvtChanged( uint32_t iTOW,
                         bool     rtkFix,
                         bool     rtkFloat,
                         uint8_t  numSatelites,
                         double   speed,
                         double   headingOfMotion,
                         double   headingOfVehicle,
                         double   pDOP );
};

class UbxParser : public BlockBase {
  Q_OBJECT

public:
  explicit UbxParser();

Q_SIGNALS:
  void globalPositionChanged( VECTOR_SIGNATURE_SIGNAL );
  void velocityChanged( NUMBER_SIGNATURE_SIGNAL );

  void orientationDualAntennaChanged( ORIENTATION_SIGNATURE_SIGNAL );
  void orientationVehicleChanged( ORIENTATION_SIGNATURE_SIGNAL );
  void orientationMotionChanged( ORIENTATION_SIGNATURE_SIGNAL );

  void distanceBetweenAntennasChanged( NUMBER_SIGNATURE_SIGNAL );
  void numSatelitesChanged( NUMBER_SIGNATURE_SIGNAL );
  void hdopChanged( NUMBER_SIGNATURE_SIGNAL );
  void horizontalAccuracyChanged( NUMBER_SIGNATURE_SIGNAL );
  void verticalAccuracyChanged( NUMBER_SIGNATURE_SIGNAL );

public Q_SLOTS:
  void setData( const QByteArray& data );

protected Q_SLOTS:
  void ubxNavHpPosLLH( uint32_t /*iTOW*/, double lon, double lat, double height, double /*hMSL*/, double hAcc, double vAcc );

  void
  ubxNavRelPosNed( uint32_t /*iTOW*/, double /*relPosN*/, double /*relPosE*/, double relPosD, double relPosLenght, double relPosHeading );

  void UbxNavPvt( uint32_t /*iTOW*/,
                  bool /*rtkFix*/,
                  bool /*rtkFloat*/,
                  uint8_t numSatelites,
                  double  speed,
                  double  headingOfMotion,
                  double  headingOfVehicle,
                  double  pDOP );

  void setHeadingOffset( NUMBER_SIGNATURE_SLOT );
  void setHeadingFactor( NUMBER_SIGNATURE_SLOT );

  void setRollOffset( NUMBER_SIGNATURE_SLOT );
  void setRollFactor( NUMBER_SIGNATURE_SLOT );

private:
  UBX_Parser_Helper ubxParser;
  double            headingOffsetRad = 0;
  double            rollOffsetRad    = 0;
  double            headingFactor    = 1;
  double            rollFactor       = 1;
};

class UbxParserFactory : public BlockFactory {
  Q_OBJECT

public:
  UbxParserFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "UBX Parser" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Parsers" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
