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
  explicit ValueTransmissionBase64Data( int id );

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
  ValueTransmissionBase64DataFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Value Transmit Base64 Data" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Value Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
