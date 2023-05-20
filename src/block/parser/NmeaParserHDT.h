// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"
#include "helpers/eigenHelper.h"

class NmeaParserHDT : public BlockBase {
  Q_OBJECT

public:
  explicit NmeaParserHDT() : BlockBase() {}

Q_SIGNALS:
  void orientationChanged( ORIENTATION_SIGNATURE_SIGNAL );

public Q_SLOTS:
  void setData( const QByteArray& data );

public:
  void parseData();

public:
private:
  QByteArray dataToParse;
};

class NmeaParserHDTFactory : public BlockFactory {
  Q_OBJECT

public:
  NmeaParserHDTFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "NMEA Parser HDT" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Parsers" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "NMEA Parser for HDT sentences" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
