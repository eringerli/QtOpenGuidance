// Copyright( C ) 2019 Christian Riggenbach
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

#include "GuidanceBase.h"

#include "../kinematic/Tile.h"

#include "qneblock.h"
#include "qneport.h"

#include <QDebug>

class DebugSink : public GuidanceBase {
    Q_OBJECT

  public:
    explicit DebugSink()
      : GuidanceBase(),
        block( nullptr ) {}

  public slots:
    void setPosition( QVector3D value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->name << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setTiledPosition( Tile* tile, QVector3D value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->name << "Tile:" << tile << "(" << tile->x << "|" << tile->y << ")" << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << "Tile:" << tile << "(" << tile->x << "|" << tile->y << ")" << value;
      }
    }

    void setWGS84Position( double latitude, double longitude, double height ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->name << latitude << longitude << height;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << latitude << longitude << height;
      }
    }

    void setOrientation( QQuaternion value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->name << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setSteeringAngle( float value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->name << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setPose( Tile* tile, QVector3D position, QQuaternion orientation ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->name << "Tile:" << tile << "(" << tile->x << "|" << tile->y << ")" << position << orientation;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << "Tile:" << tile << "(" << tile->x << "|" << tile->y << ")"  << position << orientation;
      }
    }

    void setData( QByteArray data ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->name << "Data:" << data;
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

class DebugSinkFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    DebugSinkFactory()
      : GuidanceFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Console Output" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new DebugSink;
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      auto debugSink = qobject_cast<DebugSink*>( obj );

      if( debugSink ) {
        debugSink->block = b;
      }

      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "WGS84 Position", SLOT( setWGS84Position( double, double, double ) ) );
      b->addInputPort( "Position", SLOT( setPosition( QVector3D ) ) );
      b->addInputPort( "Tiled Position", SLOT( setTiledPosition( Tile*, QVector3D ) ) );
      b->addInputPort( "Orientation", SLOT( setOrientation( QQuaternion ) ) );
      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion ) ) );
      b->addInputPort( "Steering Angle", SLOT( setSteeringAngle( float ) ) );
      b->addInputPort( "Data", SLOT( setData( QByteArray ) ) );

      return b;
    }
};

#endif // DEBUGSINK_H

