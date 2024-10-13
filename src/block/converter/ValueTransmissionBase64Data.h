// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "ValueTransmissionBase.h"

#include <memory>

class QCborStreamReader;

class ValueTransmissionBase64Data : public ValueTransmissionBase {
  Q_OBJECT
public:
  explicit ValueTransmissionBase64Data( uint16_t cid, const int idHint, const bool systemBlock, const QString type );

public Q_SLOTS:
  void setData( const QByteArray& data );

  void dataReceive( const QByteArray& data );

Q_SIGNALS:
  void dataToSend( const QByteArray& );
  void dataChanged( const QByteArray& );

private:
  std::unique_ptr< QCborStreamReader > reader;
};

class ValueTransmissionBase64DataFactory : public BlockFactory {
  Q_OBJECT

public:
  ValueTransmissionBase64DataFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Converter; }

  QString getNameOfFactory() const override { return QStringLiteral( "Value Transmit Base64 Data" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Value Converters" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
