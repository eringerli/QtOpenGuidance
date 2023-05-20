// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "ValueTransmissionBase.h"

#include <memory>

#include "helpers/eigenHelper.h"

class QCborStreamReader;

class ValueTransmissionQuaternion : public ValueTransmissionBase {
  Q_OBJECT
public:
  explicit ValueTransmissionQuaternion( uint16_t cid );

public Q_SLOTS:
  void setQuaternion( ORIENTATION_SIGNATURE_SLOT );

  void dataReceive( const QByteArray& data );

Q_SIGNALS:
  void dataToSend( const QByteArray& );
  void quaternionChanged( ORIENTATION_SIGNATURE_SIGNAL );

private:
  std::unique_ptr< QCborStreamReader > reader;
};

class ValueTransmissionQuaternionFactory : public BlockFactory {
  Q_OBJECT

public:
  ValueTransmissionQuaternionFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Value Transmit Quaternion" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Value Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
