// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "ValueTransmissionBase.h"

#include <memory>

#include <QCborStreamReader>

class ValueTransmissionState : public ValueTransmissionBase {
  Q_OBJECT
public:
  explicit ValueTransmissionState( uint16_t cid, const BlockBaseId idHint, const bool systemBlock, const QString type );

public Q_SLOTS:
  void setState( ACTION_SIGNATURE_SLOT );

  void dataReceive( const QByteArray& data );

Q_SIGNALS:
  void dataToSend( const QByteArray& );
  void stateChanged( const bool );

private:
  std::unique_ptr< QCborStreamReader > reader;
};

class ValueTransmissionStateFactory : public BlockFactory {
  Q_OBJECT

public:
  ValueTransmissionStateFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Converter; }

  QString getNameOfFactory() const override { return QStringLiteral( "Value Transmit State" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Value Converters" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
