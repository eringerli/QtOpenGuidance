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
  explicit ValueTransmissionState( int id );

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
  ValueTransmissionStateFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Value Transmit State" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Value Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
