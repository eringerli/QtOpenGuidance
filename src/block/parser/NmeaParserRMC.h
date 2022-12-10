// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class NmeaParserRMC : public BlockBase {
  Q_OBJECT

public:
  explicit NmeaParserRMC() : BlockBase() {}

Q_SIGNALS:
  void globalPositionChanged( const Eigen::Vector3d& );
  void velocityChanged( NUMBER_SIGNATURE_SIGNAL );

public Q_SLOTS:
  void setData( const QByteArray& data );

public:
  void parseData();

private:
  QByteArray dataToParse;
};

class NmeaParserRMCFactory : public BlockFactory {
  Q_OBJECT

public:
  NmeaParserRMCFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "NMEA Parser RMC" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Parsers" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "NMEA Parser for RMC sentences" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
