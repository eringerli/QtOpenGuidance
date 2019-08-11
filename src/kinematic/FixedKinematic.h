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

#ifndef TRACTORKINEMATIC_H
#define TRACTORKINEMATIC_H

#include <QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QQuaternion>
#include <QVector3D>

#include <QtGlobal>
#include <QtDebug>

#include "../block/BlockBase.h"

#include "../kinematic/Tile.h"

class FixedKinematic : public BlockBase {
    Q_OBJECT

  public:
    explicit FixedKinematic()
      : BlockBase(),
        m_offsetHookPoint( QVector3D( 0, 0, 0 ) ), m_offsetTowPoint( QVector3D( -1, 0, 0 ) ) {}

  public slots:
    void setOffsetTowPointPosition( QVector3D position ) {
      m_offsetTowPoint = position;
    }
    void setOffsetHookPointPosition( QVector3D position ) {
      m_offsetHookPoint = position;
    }

    void setPose( Tile* tile, QVector3D position, QQuaternion orientation ) {
      QVector3D positionPivotPoint = position + orientation * -m_offsetHookPoint;
      QVector3D positionTowPoint = positionPivotPoint + orientation * m_offsetTowPoint;

      emit poseHookPointChanged( tile, position, orientation );

      Tile* currentTile = tile->getTileForPosition( &positionTowPoint );
      emit poseTowPointChanged( currentTile, positionTowPoint, orientation );

      currentTile = tile->getTileForPosition( &positionPivotPoint );
      emit posePivotPointChanged( currentTile, positionPivotPoint, orientation );
    }

  signals:
    void poseHookPointChanged( Tile*, QVector3D, QQuaternion );
    void posePivotPointChanged( Tile*, QVector3D, QQuaternion );
    void poseTowPointChanged( Tile*, QVector3D, QQuaternion );

  private:
    // defined in the normal way: x+ is forwards, so m_offsetPivotPoint is a negative vector
    QVector3D m_offsetHookPoint;
    QVector3D m_offsetTowPoint;
};

class FixedKinematicFactory : public BlockFactory {
    Q_OBJECT

  public:
    FixedKinematicFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Fixed Kinematic" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new FixedKinematic;
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "OffsetHookPoint", SLOT( setOffsetHookPointPosition( QVector3D ) ) );
      b->addInputPort( "OffsetTowPoint", SLOT( setOffsetTowPointPosition( QVector3D ) ) );
      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion ) ) );

      b->addOutputPort( "Pose Hook Point", SIGNAL( poseHookPointChanged( Tile*, QVector3D, QQuaternion ) ) );
      b->addOutputPort( "Pose Pivot Point", SIGNAL( posePivotPointChanged( Tile*, QVector3D, QQuaternion ) ) );
      b->addOutputPort( "Pose Tow Point", SIGNAL( poseTowPointChanged( Tile*, QVector3D, QQuaternion ) ) );

      return b;
    }
};

#endif // TRACTORKINEMATIC_H
