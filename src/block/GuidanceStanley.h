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

#ifndef STANLEYGUIDANCE_H
#define STANLEYGUIDANCE_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"

#include <QVector>
#include <QSharedPointer>
#include "PathPrimitive.h"

class StanleyGuidance : public BlockBase {
    Q_OBJECT

  public:
    explicit StanleyGuidance( Tile* tile )
      : BlockBase() {
      this->tile = tile->getTileForOffset( 0, 0 );
    }

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion orientation ) {
      this->tile = tile;
      this->position = position;
      this->orientation = orientation;

      double distance = qInf();

      for( auto primitive : plan ) {
        PathPrimitiveLine* line =  qobject_cast<PathPrimitiveLine*>( primitive.data() );

        if( line ) {
          double distanceTmp = lineToPointDistance2D(
                                 line->x1, line->y1,
                                 line->x2, line->y2,
                                 tile->x + double( position.x() ), tile->y + double( position.y() ),
                                 line->segment
                               );
          qDebug() << "tmp:" << distanceTmp << line->x1 << line->y1 <<
                   line->x2 << line->y2 << line->segment <<
                   tile->x + double( position.x() ) << tile->y + double( position.y() );

          distance = qMin( distance, distanceTmp );
        }
      }

      qDebug() << "result:" << distance;
      emit xteChanged( distance );
    }

    void setPlan( QVector<QSharedPointer<PathPrimitive>> plan ) {
      this->plan = plan;
    }

  signals:
    void xteChanged( double );

  private:

    // https://stackoverflow.com/a/4448097

    // Compute the dot product AB . BC
    double dotProduct( double aX, double aY, double bX, double bY, double cX, double cY ) {
      return ( bX - aX ) * ( cX - bX ) + ( bY - aY ) * ( cY - bY );
    }

    // Compute the cross product AB x AC
    double crossProduct( double aX, double aY, double bX, double bY, double cX, double cY ) {
      return ( bX - aX ) * ( cY - aY ) - ( bY - aY ) * ( cX - aX );
    }

    // Compute the distance from A to B
    double distance( double aX, double aY, double bX, double bY ) {
      double d1 = aX - bX;
      double d2 = aY - bY;

      return qSqrt( d1 * d1 + d2 * d2 );
    }

    // Compute the distance from AB to C
    // if isSegment is true, AB is a segment, not a line.
    // if <0: left side of line
    double lineToPointDistance2D( double aX, double aY, double bX, double bY, double cX, double cY, bool isSegment ) {
      double dist = crossProduct( aX, aY, bX, bY, cX, cY ) / distance( aX, aY, bX, bY );

      if( isSegment ) {
        if( dotProduct( aX, aY, bX, bY, cX, cY ) > 0 ) {
          return distance( bX, bY, cX, cY );
        }

        if( dotProduct( bX, bY, aX, aY, cX, cY ) > 0 ) {
          return distance( aX, aY, cX, cY );
        }
      }

      return dist;
    }

  public:
    Tile* tile = nullptr;
    QVector3D position = QVector3D();
    QQuaternion orientation = QQuaternion();

  private:
    QVector<QSharedPointer<PathPrimitive>> plan;
};

class StanleyGuidanceFactory : public BlockFactory {
    Q_OBJECT

  public:
    StanleyGuidanceFactory( Tile* tile )
      : BlockFactory(),
        tile( tile ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Stanley Path Follower" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new StanleyGuidance( tile );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj, true );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion ) ) );
      b->addInputPort( "Plan", SLOT( setPlan( QVector<QSharedPointer<PathPrimitive>> ) ) );

      b->addOutputPort( "XTE", SIGNAL( xteChanged( double ) ) );

      return b;
    }

  private:
    Tile* tile;
};

#endif // STANLEYGUIDANCE_H

