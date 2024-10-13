// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class NmeaParserGGA : public BlockBase {
  Q_OBJECT

public:
  explicit NmeaParserGGA( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

Q_SIGNALS:
  void globalPositionChanged( VECTOR_SIGNATURE_SLOT );
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
  NmeaParserGGAFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Parser; }

  QString getNameOfFactory() const override { return QStringLiteral( "NMEA Parser GGA/GNS" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Parsers" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "NMEA Parser for GGA/GNS sentences" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
