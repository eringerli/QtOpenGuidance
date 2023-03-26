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
  explicit ValueTransmissionImuData( uint16_t cid );

public Q_SLOTS:
  void setImuData( const double dT, const Eigen::Vector3d&, const Eigen::Vector3d&, const Eigen::Vector3d& );

  void dataReceive( const QByteArray& data );

Q_SIGNALS:
  void dataToSend( const QByteArray& );
  void imuDataChanged( const double dT, const Eigen::Vector3d&, const Eigen::Vector3d&, const Eigen::Vector3d& );

private:
  std::unique_ptr< QCborStreamReader > reader;

  uint32_t lastTimestamp;
};

class ValueTransmissionImuDataFactory : public BlockFactory {
  Q_OBJECT

public:
  ValueTransmissionImuDataFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Value Transmit IMU Data" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Value Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
