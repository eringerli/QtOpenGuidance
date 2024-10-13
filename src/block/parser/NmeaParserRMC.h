// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class NmeaParserRMC : public BlockBase {
  Q_OBJECT

public:
  explicit NmeaParserRMC( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

Q_SIGNALS:
  void globalPositionChanged( VECTOR_SIGNATURE_SLOT );
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
  NmeaParserRMCFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Parser; }

  QString getNameOfFactory() const override { return QStringLiteral( "NMEA Parser RMC" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Parsers" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "NMEA Parser for RMC sentences" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
