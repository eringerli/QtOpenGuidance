// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gui/model/VectorBlockModel.h"

#pragma once

#include <QObject>

#include <Eigen/Geometry>
#include <QVector3D>

#include "block/BlockBase.h"

class VectorBlock : public BlockBase {
  Q_OBJECT

public:
  explicit VectorBlock( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

  void emitConfigSignals() override;

  void toJSON( QJsonObject& json ) const override;
  void fromJSON( const QJsonObject& ) override;

Q_SIGNALS:
  void vectorChanged( VECTOR_SIGNATURE_SLOT );

public:
  Eigen::Vector3d vector = Eigen::Vector3d( 0, 0, 0 );
};

class VectorBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  VectorBlockFactory( QThread* thread, VectorBlockModel* model ) : BlockFactory( thread, false ), model( model ) {
    typeColor = TypeColor::Value;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Vector3D" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Literals" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "3D Vector" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  VectorBlockModel* model = nullptr;
};
