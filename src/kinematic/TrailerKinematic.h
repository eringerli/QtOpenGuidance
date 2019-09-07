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

#ifndef TRAILERKINEMATIC_H
#define TRAILERKINEMATIC_H

#include <QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QQuaternion>
#include <QVector3D>

#include <QtGlobal>
#include <QtMath>
#include <QtDebug>

#include "../block/BlockBase.h"

#include "../kinematic/Tile.h"
#include "../kinematic/PoseOptions.h"

class TrailerKinematic : public BlockBase {
    Q_OBJECT

  public:
    explicit TrailerKinematic( Tile* tile )
      : BlockBase() {
      m_tilePivotPoint = tile->getTileForOffset( 0, 0 );
    }

  public slots:
    void setOffsetTowPointPosition( QVector3D position ) {
      m_offsetTowPoint = position;
    }
    void setOffsetHookPointPosition( QVector3D position ) {
      m_offsetHookPoint = position;
    }

    void setMaxJackknifeAngle( float maxAngle ) {
      m_maxJackknifeAngle = maxAngle;
    }

    void setPose( Tile* tile, QVector3D position, QQuaternion orientation, PoseOption::Options options ) {
      QQuaternion orientationTrailer;

      if( options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        orientation = QQuaternion();
      } else {
        orientationTrailer = QQuaternion::fromAxisAndAngle(
                               QVector3D( 0.0f, 0.0f, 1.0f ),
                               qRadiansToDegrees( qAtan2(
                                     position.y() - m_positionPivotPoint.y() + ( tile->y - m_tilePivotPoint->y ),
                                     position.x() - m_positionPivotPoint.x() + ( tile->x - m_tilePivotPoint->x ) ) )
                             );

        // the angle between tractor and trailer >m_maxAngleToTowingKinematic -> reset orientation to the one from the tractor
        if( qAbs( ( orientation.inverted()*orientationTrailer ).toEulerAngles().z() ) < m_maxJackknifeAngle ) {
          orientation = orientationTrailer;
        }
      }

      emit poseHookPointChanged( tile, position, orientation, options );

      QVector3D positionPivotPoint = position + ( orientation * -m_offsetHookPoint );
      Tile* tilePivotPoint;

      if( options.testFlag( PoseOption::CalculateWithoutTiling ) ) {
        tilePivotPoint = tile;
      } else {
        tilePivotPoint = tile->getTileForPosition( &positionPivotPoint );
      }

      QVector3D positionTowPoint = positionPivotPoint + ( orientation * m_offsetTowPoint );

      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        m_positionPivotPoint = positionPivotPoint;
        m_tilePivotPoint = tilePivotPoint;
      }

      Tile* currentTile;

      if( options.testFlag( PoseOption::CalculateWithoutTiling ) ) {
        currentTile = tile;
      } else {
        currentTile = tile->getTileForPosition( &positionTowPoint );
      }

      emit poseTowPointChanged( currentTile, positionTowPoint, orientation, options );

      emit posePivotPointChanged( tilePivotPoint, positionPivotPoint, orientation, options );

    }

  signals:
    void poseHookPointChanged( Tile*, QVector3D, QQuaternion, PoseOption::Options );
    void posePivotPointChanged( Tile*, QVector3D, QQuaternion, PoseOption::Options );
    void poseTowPointChanged( Tile*, QVector3D, QQuaternion, PoseOption::Options );

  private:
    // defined in the normal way: x+ is forwards, so m_offsetPivotPoint is a negative vector
    QVector3D m_offsetHookPoint = QVector3D( 6, 0, 0 );
    QVector3D m_offsetTowPoint = QVector3D( -1, 0, 0 );
    QVector3D m_positionPivotPoint = QVector3D( 0, 0, 0 );

    float m_maxJackknifeAngle = 120;

    Tile* m_tilePivotPoint;
};

class TrailerKinematicFactory : public BlockFactory {
    Q_OBJECT

  public:
    TrailerKinematicFactory( Tile* tile )
      : BlockFactory(),
        tile( tile ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Trailer Kinematic" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new TrailerKinematic( tile );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "OffsetHookPoint", SLOT( setOffsetHookPointPosition( QVector3D ) ) );
      b->addInputPort( "OffsetTowPoint", SLOT( setOffsetTowPointPosition( QVector3D ) ) );
      b->addInputPort( "MaxJackknifeAngle", SLOT( setMaxJackknifeAngle( float ) ) );
      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );

      b->addOutputPort( "Pose Hook Point", SIGNAL( poseHookPointChanged( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addOutputPort( "Pose Pivot Point", SIGNAL( posePivotPointChanged( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addOutputPort( "Pose Tow Point", SIGNAL( poseTowPointChanged( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );

      return b;
    }

  private:
    Tile* tile;
};

#endif // TRAILERKINEMATIC_H
