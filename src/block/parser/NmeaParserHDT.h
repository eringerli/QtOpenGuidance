// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"
#include "helpers/eigenHelper.h"

class NmeaParserHDT : public BlockBase {
  Q_OBJECT

public:
  explicit NmeaParserHDT( const BlockBaseId idHint, const bool systemBlock, const QString type, const BlockBase::TypeColor typeColor )
      : BlockBase( idHint, systemBlock, type, typeColor ) {}

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
  NmeaParserHDTFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = BlockBase::TypeColor::Parser; }

  QString getNameOfFactory() const override { return QStringLiteral( "NMEA Parser HDT" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Parsers" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "NMEA Parser for HDT sentences" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
