// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "ValueTransmissionBase.h"

#include <memory>

class QCborStreamReader;

class ValueTransmissionNumber : public ValueTransmissionBase {
  Q_OBJECT
public:
  explicit ValueTransmissionNumber( uint16_t cid, const int idHint, const bool systemBlock, const QString type );

public Q_SLOTS:
  void setNumber( NUMBER_SIGNATURE_SLOT );
  void dataReceive( const QByteArray& );

Q_SIGNALS:
  void dataToSend( const QByteArray& );
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  std::unique_ptr< QCborStreamReader > reader;
};

class ValueTransmissionNumberFactory : public BlockFactory {
  Q_OBJECT

public:
  ValueTransmissionNumberFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Converter; }

  QString getNameOfFactory() const override { return QStringLiteral( "Value Transmit Number" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Value Converters" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
