// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class NmeaParserGGA : public BlockBase {
  Q_OBJECT

public:
  explicit NmeaParserGGA() : BlockBase() {}

Q_SIGNALS:
  void globalPositionChanged( const Eigen::Vector3d& );
  void fixQualityChanged( NUMBER_SIGNATURE_SIGNAL );
  void hdopChanged( NUMBER_SIGNATURE_SIGNAL );
  void numSatelitesChanged( NUMBER_SIGNATURE_SIGNAL );
  void ageOfDifferentialDataChanged( NUMBER_SIGNATURE_SIGNAL );

public Q_SLOTS:
  void setData( const QByteArray& data );

public:
  void parseData();

private:
  QByteArray dataToParse;
};

class NmeaParserGGAFactory : public BlockFactory {
  Q_OBJECT

public:
  NmeaParserGGAFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "NMEA Parser GGA/GNS" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Parsers" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "NMEA Parser for GGA/GNS sentences" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
