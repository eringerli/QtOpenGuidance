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

#ifndef XTEGUIDANCE_H
#define XTEGUIDANCE_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>

// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

class XteGuidance : public BlockBase {
    Q_OBJECT

  public:
    explicit XteGuidance( Tile* tile )
      : BlockBase() {
      this->tile = tile->getTileForOffset( 0, 0 );
    }

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {

        double distance = qInf();
        double headingOfABLine = 0;

        for( const auto& primitive : plan ) {
          auto* line =  qobject_cast<PathPrimitiveLine*>( primitive.data() );

          if( line ) {
            double distanceTmp = lineToPointDistance2D(
                                   line->x1, line->y1,
                                   line->x2, line->y2,
                                   tile->x + double( position.x() ), tile->y + double( position.y() ),
                                   line->isSegment
                                 );

            if( distanceTmp < distance ) {
              headingOfABLine = qAtan2( line->y1 - line->y2, line->x1 - line->x2 ) - M_PI;
              distance = distanceTmp;
            }
          }
        }

        if( !qIsInf( distance ) ) {
          emit headingOfPathChanged( float( headingOfABLine ) );
          emit xteChanged( float( distance ) );
        } else {
          emit headingOfPathChanged( qInf() );
          emit xteChanged( qInf() );
        }
      }
    }

    void setPlan( QVector<QSharedPointer<PathPrimitive>> plan ) {
      this->plan = std::move( plan );
    }

    void emitConfigSignals() override {
//      emit xteChanged( 0 );
//      emit headingOfPathChanged(0);
    }

  signals:
    void xteChanged( float );
    void headingOfPathChanged( float );

  private:

    double normalizeAngleRadians( double angle ) {
      while( angle > M_PI ) {
        angle -= M_PI * 2;
      }

      while( angle < -M_PI ) {
        angle += M_PI * 2;
      }

      return angle;
    }

    double normalizeAngleDegrees( double angle ) {
      while( angle > 180 ) {
        angle -= 360;
      }

      while( angle < -180 ) {
        angle += 360;
      }

      return angle;
    }

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

class XteGuidanceFactory : public BlockFactory {
    Q_OBJECT

  public:
    XteGuidanceFactory( Tile* tile )
      : BlockFactory(),
        tile( tile ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Cross Track Error" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new XteGuidance( tile );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "Plan", SLOT( setPlan( QVector<QSharedPointer<PathPrimitive>> ) ) );

      b->addOutputPort( "XTE", SIGNAL( xteChanged( float ) ) );
      b->addOutputPort( "Heading of Path", SIGNAL( headingOfPathChanged( float ) ) );

      return b;
    }

  private:
    Tile* tile;
};

#endif // XTEGUIDANCE_H

