// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QCborStreamReader>
#include <QObject>

#include "ValueTransmissionBase.h"

#include <memory>

#include "helpers/eigenHelper.h"

class QCborStreamReader;

class ValueTransmissionImuData : public ValueTransmissionBase {
  Q_OBJECT
public:
  explicit ValueTransmissionImuData(
    uint16_t cid, const BlockBaseId idHint, const bool systemBlock, const QString type, const BlockBase::TypeColor typeColor );

public Q_SLOTS:
  void setImuData( IMU_SIGNATURE_SLOT );

  void dataReceive( const QByteArray& data );

Q_SIGNALS:
  void dataToSend( const QByteArray& );
  void imuDataChanged( IMU_SIGNATURE_SIGNAL );

private:
  std::unique_ptr< QCborStreamReader > reader;

  uint32_t lastTimestamp;
};

class ValueTransmissionImuDataFactory : public BlockFactory {
  Q_OBJECT

public:
  ValueTransmissionImuDataFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = BlockBase::TypeColor::Converter; }

  QString getNameOfFactory() const override { return QStringLiteral( "Value Transmit IMU Data" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Value Converters" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
