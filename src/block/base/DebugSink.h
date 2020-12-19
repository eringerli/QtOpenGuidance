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

#include <QDateTime>

#include <QByteArray>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

#include "qneblock.h"
#include "qneport.h"

#include <QDebug>

class DebugSink : public BlockBase {
    Q_OBJECT

  public:
    explicit DebugSink()
      : BlockBase(),
        block( nullptr ) {}

  public Q_SLOTS:
    void setPosition( QVector3D value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setWGS84Position( double latitude, double longitude, double height ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << latitude << longitude << height;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << latitude << longitude << height;
      }
    }

    void setOrientation( const Eigen::Quaterniond& value ) {
      auto taitBryan = quaternionToTaitBryan( value );

      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << taitBryan.y() << taitBryan.x() << taitBryan.z();
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << taitBryan.y() << taitBryan.x() << taitBryan.z();
      }
    }

    void setSteeringAngle( double value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
      auto taitBryan = quaternionToTaitBryan( orientation );

      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << position.x() << position.y() << position.z() << taitBryan.y() << taitBryan.x() << taitBryan.z() << options;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << position.x() << position.y() << position.z() << taitBryan.y() << taitBryan.x() << taitBryan.z() << options;
      }
    }

    void setData( const QByteArray& data ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << "Data:" << data;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << "Data:" << data;
      }
    }
  public:
    virtual void emitConfigSignals() override {
    }

  public:
    QNEBlock* block;
};

class DebugSinkFactory : public BlockFactory {
    Q_OBJECT

  public:
    DebugSinkFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Console Output" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Base Blocks" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new DebugSink;
      auto* b = createBaseBlock( scene, obj, id );

      auto debugSink = qobject_cast<DebugSink*>( obj );

      if( debugSink ) {
        debugSink->block = b;
      }

      b->addInputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SLOT( setWGS84Position( double, double, double ) ) ) );
      b->addInputPort( QStringLiteral( "Position" ), QLatin1String( SLOT( setPosition( Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( Eigen::Quaterniond ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( double ) ) ) );
      b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ) );

      return b;
    }
};
