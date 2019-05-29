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

#include <QtCore/QObject>

#include <QQuaternion>
#include <QVector3D>

#include <QDateTime>

#include "GuidanceBase.h"

#include "qneblock.h"
#include "qneport.h"

#include <QDebug>

class DebugSink : public GuidanceBase {
    Q_OBJECT

  public:
    explicit DebugSink()
      : GuidanceBase(),
        block( nullptr ) {}
    ~DebugSink() {}

  public slots:
    void setPosition( QVector3D value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setOrientation( QQuaternion value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setSteeringAngle( float value ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << value;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
      }
    }

    void setPose( QVector3D position, QQuaternion orientation ) {
      if( block ) {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << position << orientation;
      } else {
        qDebug() << QDateTime::currentMSecsSinceEpoch() << position << orientation;
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
    ~DebugSinkFactory() {}

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( QStringLiteral( "Console Output" ), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new DebugSink;
    }

    virtual void createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      auto debugSink = qobject_cast<DebugSink*>( obj );

      if( debugSink ) {
        debugSink->block = b;
      }

      scene->addItem( b );

      b->addPort( "Console Output", "", 0, QNEPort::NamePort );
      b->addPort( "Console Output", "", 0, QNEPort::TypePort );

      b->addInputPort( "Position", SLOT( setPosition( QVector3D ) ) );
      b->addInputPort( "Orientation", SLOT( setOrientation( QQuaternion ) ) );
      b->addInputPort( "Pose", SLOT( setPose( QVector3D, QQuaternion ) ) );
      b->addInputPort( "Steering Angle", SLOT( setSteeringAngle( float ) ) );
    }
};

#endif // DEBUGSINK_H

