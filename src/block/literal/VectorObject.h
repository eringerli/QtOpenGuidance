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

  void toJSON( QJsonObject& json ) override;
  void fromJSON( QJsonObject& json ) override;

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
