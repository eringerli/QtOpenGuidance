// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/GeographicConvertionWrapper.h"
#include "helpers/eigenHelper.h"

#include <QDebug>

class TransverseMercatorConverter : public BlockBase {
  Q_OBJECT

public:
  explicit TransverseMercatorConverter( GeographicConvertionWrapper* tmw, const int idHint, const bool systemBlock, const QString type )
      : BlockBase( idHint, systemBlock, type ), tmw( tmw ) {}

public Q_SLOTS:
  void setWGS84Position( VECTOR_SIGNATURE_SLOT );

Q_SIGNALS:
  void positionChanged( VECTOR_SIGNATURE_SIGNAL );

public:
  virtual void emitConfigSignals() override;

public:
  GeographicConvertionWrapper* tmw = nullptr;
};

class TransverseMercatorConverterFactory : public BlockFactory {
  Q_OBJECT

public:
  TransverseMercatorConverterFactory( QThread* thread, GeographicConvertionWrapper* tmw ) : BlockFactory( thread, false ), tmw( tmw ) {
    typeColor = TypeColor::Arithmetic;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Transverse Mercator" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Calculations" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;

private:
  GeographicConvertionWrapper* tmw = nullptr;
};
