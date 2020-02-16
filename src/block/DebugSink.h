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

#ifndef DEBUGSINK_H
#define DEBUGSINK_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include <QDateTime>

#include <QByteArray>

#include "BlockBase.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"

#include "qneblock.h"
#include "qneport.h"

#include <QDebug>

class DebugSink : public BlockBase {
    Q_OBJECT

  public:
    explicit DebugSink()
      : BlockBase(),
        block( nullptr ) {}

  public slots:
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

    void setOrientation( QQuaternion value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setSteeringAngle( double value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << position.x() << position.y() << position.z() << orientation << options;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << position.x() << position.y() << position.z() << orientation << options;
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

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new DebugSink;
      auto* b = createBaseBlock( scene, obj, id );

      auto debugSink = qobject_cast<DebugSink*>( obj );

      if( debugSink ) {
        debugSink->block = b;
      }

      b->addInputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SLOT( setWGS84Position( double, double, double ) ) ) );
      b->addInputPort( QStringLiteral( "Position" ), QLatin1String( SLOT( setPosition( QVector3D ) ) ) );
      b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( QQuaternion ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( double ) ) ) );
      b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ) );

      return b;
    }
};

#endif // DEBUGSINK_H

