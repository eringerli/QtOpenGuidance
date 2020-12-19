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
#include <QVector3D>

#include "block/BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"

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

  public Q_SLOTS:
    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const PoseOption::Options& options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        const Point_2 position2D = to2D( position );

        if( !plan.plan->empty() ) {
          double distanceSquared = qInf();
          std::shared_ptr<PathPrimitive> nearestPrimitive = nullptr;

          for( const auto& pathPrimitive : *plan.plan ) {
            if( plan.type == Plan::Type::OnlyLines || pathPrimitive->isOn( position2D ) ) {
              double currentDistanceSquared = pathPrimitive->distanceToPointSquared( position2D );

              if( currentDistanceSquared < distanceSquared ) {
                nearestPrimitive = pathPrimitive;
                distanceSquared = currentDistanceSquared;
              } else {
                if( plan.type == Plan::Type::OnlyLines ) {
                  // the plan is ordered, so we can take the fast way out...
                  break;
                }
              }
            }
          }

          if( nearestPrimitive ) {
            double offsetDistance = std::sqrt( distanceSquared ) * nearestPrimitive->offsetSign( position2D );

            Q_EMIT headingOfPathChanged( nearestPrimitive->angleAtPointDegrees( position2D ) );
            Q_EMIT xteChanged( offsetDistance );
            Q_EMIT passNumberChanged( nearestPrimitive->passNumber );
            return;
          }
        }

        Q_EMIT headingOfPathChanged( qInf() );
        Q_EMIT xteChanged( qInf() );
        Q_EMIT passNumberChanged( qInf() );
      }
    }

    void setPlan( const Plan& plan ) {
      this->plan = plan;
    }

    void emitConfigSignals() override {
      Q_EMIT xteChanged( qInf() );
      Q_EMIT headingOfPathChanged( qInf() );
      Q_EMIT passNumberChanged( qInf() );
    }

  Q_SIGNALS:
    void xteChanged( const double );
    void headingOfPathChanged( const double );
    void passNumberChanged( const double );

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

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Guidance" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new XteGuidance();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );

      b->addOutputPort( QStringLiteral( "XTE" ), QLatin1String( SIGNAL( xteChanged( const double ) ) ) );
      b->addOutputPort( QStringLiteral( "Heading of Path" ), QLatin1String( SIGNAL( headingOfPathChanged( const double ) ) ) );
      b->addOutputPort( QStringLiteral( "Pass #" ), QLatin1String( SIGNAL( passNumberChanged( const double ) ) ) );

      return b;
    }
};
