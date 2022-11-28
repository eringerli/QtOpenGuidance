// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gui/model/VectorBlockModel.h"

#pragma once

#include <QObject>

#include <Eigen/Geometry>
#include <QVector3D>

#include "block/BlockBase.h"

class VectorObject : public BlockBase {
  Q_OBJECT

public:
  explicit VectorObject() {}

  void emitConfigSignals() override;

  QJsonObject toJSON() override;
  void        fromJSON( QJsonObject& ) override;

Q_SIGNALS:
  void vectorChanged( const Eigen::Vector3d& );

public:
  Eigen::Vector3d vector = Eigen::Vector3d( 0, 0, 0 );
};

class VectorFactory : public BlockFactory {
  Q_OBJECT

public:
  VectorFactory( QThread* thread, VectorBlockModel* model ) : BlockFactory( thread ), model( model ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Vector3D" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Literals" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "3D Vector" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  VectorBlockModel* model = nullptr;
};
