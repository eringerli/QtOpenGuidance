// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class CommunicationJrk : public BlockBase {
  Q_OBJECT

public:
  explicit CommunicationJrk( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

Q_SIGNALS:
  void dataReceived( const QByteArray& );

public Q_SLOTS:
  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void setSteerZero( NUMBER_SIGNATURE_SLOT );

  void setSteerCountPerDegree( NUMBER_SIGNATURE_SLOT );

private:
  float steerZero       = 2047;
  float countsPerDegree = 45;
};

class CommunicationJrkFactory : public BlockFactory {
  Q_OBJECT

public:
  CommunicationJrkFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Converter; }

  QString getNameOfFactory() const override { return QStringLiteral( "Communication JRK" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Legacy Converters" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
