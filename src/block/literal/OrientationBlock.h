// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class OrientationBlockModel;

// algorythm from here: http://tbirdal.blogspot.com/2019/10/i-allocate-this-post-to-providing.html
class OrientationBlock : public BlockBase {
  Q_OBJECT

public:
  explicit OrientationBlock() {}

  void emitConfigSignals() override;

  QJsonObject toJSON() const override;
  void        fromJSON( QJsonObject& ) override;

public Q_SLOTS:
  void setAveragerEnabled( ACTION_SIGNATURE_SLOT );

  void setOrientation( ORIENTATION_SIGNATURE_SLOT );

  void setPose( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void orientationChanged( ORIENTATION_SIGNATURE_SIGNAL );

public:
  Eigen::Quaterniond orientation;
  Eigen::Vector3d    position      = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Vector3d    positionStart = Eigen::Vector3d( 0, 0, 0 );

  int  numMeasurements    = 0;
  bool averagerEnabled    = false;
  bool averagerEnabledOld = false;

private:
  Eigen::Matrix4d A;
};

class OrientationBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  OrientationBlockFactory( QThread* thread, OrientationBlockModel* model ) : BlockFactory( thread, false ), model( model ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Orientation" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Literals" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  OrientationBlockModel* model = nullptr;
};
