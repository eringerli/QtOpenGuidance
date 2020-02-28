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

class LocalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit LocalPlanner()
      : BlockBase() {}

  public slots:
    void setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation = orientation;

        const Point_2 position2D = to2D( position );

        if( !globalPlan.plan->empty() ) {

          // local planner for lines: find the nearest line and put it into the local plan
          if( globalPlan.type == Plan::Type::OnlyLines ) {
            double distanceSquared = qInf();
            typeof( globalPlan.plan->front() ) nearestLine;

            for( const auto& pathPrimitive : *globalPlan.plan ) {
              auto line = pathPrimitive->castToLine();
              double currentDistanceSquared = CGAL::squared_distance( line->line, position2D );

              if( currentDistanceSquared < distanceSquared ) {
                nearestLine = pathPrimitive;
                distanceSquared = currentDistanceSquared;
              } else {
                // the plan is ordered, so we can take the fast way out...
                break;
              }
            }

            plan.type = Plan::Type::OnlyLines;
            plan.plan->clear();
            auto line = nearestLine->castToLine();

            if( line->anyDirection ) {
              double angleNearestLine = angleOfLineDegrees( line->line );

              if( ( orientation.toEulerAngles().z() - angleNearestLine ) > 95 ) {
                nearestLine = std::make_shared<PathPrimitiveLine>(
                                      line->line.opposite(),
                                      line->implementWidth, line->anyDirection, line->passNumber );

              }
            }

            plan.plan->push_back( nearestLine );
            emit planChanged( plan );
          }
        }
      }
    }

    void setPlan( const Plan& plan ) {
      this->globalPlan = plan;
//      emit planChanged( plan );
    }


  signals:
    void planChanged( const Plan& );

  public:
    Point_3 position = Point_3();
    QQuaternion orientation = QQuaternion();

  private:
    Plan globalPlan;
    Plan plan;
};

class LocalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    LocalPlannerFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Local Planner" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new LocalPlanner();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );
      b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( const Plan& ) ) ) );

      return b;
    }
};


