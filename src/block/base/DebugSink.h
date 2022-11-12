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
