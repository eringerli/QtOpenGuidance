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

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"
#include "../kinematic/Plan.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>

// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

class XteGuidance : public BlockBase {
    Q_OBJECT

  public:
    explicit XteGuidance()
      : BlockBase() {}

  public slots:
    void setPose( const Point_3& position, const QQuaternion, const PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        const Point_2 position2D = to2D( position );

        if( !plan.plan->empty() ) {

          // local planner for lines: find the nearest line and put it into the local plan
          if( plan.type == Plan::Type::OnlyLines ) {
            double distanceSquared = qInf();
            const PathPrimitiveLine* nearestLine = nullptr;


            for( const auto& pathPrimitive : *plan.plan ) {
              auto line = pathPrimitive->castToLine();
              double currentDistanceSquared = CGAL::squared_distance( line->line, position2D );

              if( currentDistanceSquared < distanceSquared ) {
                nearestLine = line;
                distanceSquared = currentDistanceSquared;
              } else {
                // the plan is ordered, so we can take the fast way out...
                break;
              }
            }

            double offsetDistance = std::sqrt( distanceSquared );

            if( nearestLine->line.has_on_negative_side( position2D ) ) {
              offsetDistance = -offsetDistance;
            }

            emit headingOfPathChanged( angleOfLineDegrees( nearestLine->line ) );
            emit xteChanged( offsetDistance );
            emit passNumberChanged( nearestLine->passNumber );
            return;
          }
        }

        emit headingOfPathChanged( qInf() );
        emit xteChanged( qInf() );
        emit passNumberChanged( qInf() );
      }
    }

    void setPlan( const Plan& plan ) {
      this->plan = plan;
    }

    void emitConfigSignals() override {
      emit xteChanged( qInf() );
      emit headingOfPathChanged( qInf() );
      emit passNumberChanged( qInf() );
    }

  signals:
    void xteChanged( double );
    void headingOfPathChanged( double );
    void passNumberChanged( double );

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


  public:
    Point_3 position = Point_3();
    QQuaternion orientation = QQuaternion();

  private:
    Plan plan;
};

class XteGuidanceFactory : public BlockFactory {
    Q_OBJECT

  public:
    XteGuidanceFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Cross Track Error" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new XteGuidance();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );

      b->addOutputPort( QStringLiteral( "XTE" ), QLatin1String( SIGNAL( xteChanged( double ) ) ) );
      b->addOutputPort( QStringLiteral( "Heading of Path" ), QLatin1String( SIGNAL( headingOfPathChanged( double ) ) ) );
      b->addOutputPort( QStringLiteral( "Pass #" ), QLatin1String( SIGNAL( passNumberChanged( double ) ) ) );

      return b;
    }
};


