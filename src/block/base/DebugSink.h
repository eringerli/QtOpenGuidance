// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class DebugSink : public BlockBase {
  Q_OBJECT

public:
  explicit DebugSink() : BlockBase(), block( nullptr ) {}

public Q_SLOTS:
  void setPosition( QVector3D value );

  void setWGS84Position( double latitude, double longitude, double height );

  void setOrientation( const Eigen::Quaterniond& value );

  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void setPose( POSE_SIGNATURE_SLOT );

  void setData( const QByteArray& data );

public:
  QNEBlock* block;
};

class DebugSinkFactory : public BlockFactory {
  Q_OBJECT

public:
  DebugSinkFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Console Output" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Base Blocks" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
