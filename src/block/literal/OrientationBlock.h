// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

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

  void toJSON( QJsonObject& json ) override;
  void fromJSON( QJsonObject& json ) override;

public Q_SLOTS:
  void setAveragerEnabled( ACTION_SIGNATURE_SLOT );

  void setOrientation( const Eigen::Quaterniond& orientation );

  void setPose( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void orientationChanged( const Eigen::Quaterniond& );

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
  OrientationBlockFactory( QThread* thread, OrientationBlockModel* model ) : BlockFactory( thread ), model( model ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Orientation" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Literals" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  OrientationBlockModel* model = nullptr;
};
