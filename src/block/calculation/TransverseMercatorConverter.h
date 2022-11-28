// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/GeographicConvertionWrapper.h"
#include "helpers/eigenHelper.h"

#include <QDebug>

class TransverseMercatorConverter : public BlockBase {
  Q_OBJECT

public:
  explicit TransverseMercatorConverter( GeographicConvertionWrapper* tmw ) : BlockBase(), tmw( tmw ) {}

public Q_SLOTS:
  void setWGS84Position( const Eigen::Vector3d& position );

Q_SIGNALS:
  void positionChanged( const Eigen::Vector3d& );

public:
  virtual void emitConfigSignals() override;

public:
  GeographicConvertionWrapper* tmw = nullptr;
};

class TransverseMercatorConverterFactory : public BlockFactory {
  Q_OBJECT

public:
  TransverseMercatorConverterFactory( QThread* thread, GeographicConvertionWrapper* tmw ) : BlockFactory( thread ), tmw( tmw ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Transverse Mercator" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Calculations" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  GeographicConvertionWrapper* tmw = nullptr;
};
