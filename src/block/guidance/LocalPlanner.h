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

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

class MyMainWindow;
class BufferMesh;
class GuidanceTurning;

#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>

#include "kinematic/Plan.h"

#include "3d/qt3dForwards.h"

class LocalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit LocalPlanner( const QString& uniqueName, MyMainWindow* mainWindow );

    ~LocalPlanner();

  public Q_SLOTS:
    void setName( const QString& name ) override;

    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options );

    void setPlan( const Plan& plan );

    void setSteeringAngle( const double steeringAngle );

    void setPathHysteresis( const double pathHysteresis );

    void setMinRadius( const double minRadius );

    void setForceCurrentPath( const bool enabled );

    void turnLeftToggled( const bool state );
    void turnRightToggled( const bool state );
    void numSkipChanged( const int left, const int right );

  Q_SIGNALS:
    void planChanged( const Plan& );
    void triggerPlanPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options );
    void resetTurningStateOfDock();

  public:
    Eigen::Vector3d position = Eigen::Vector3d( 0, 0, 0 );
    Eigen::Quaterniond orientation = Eigen::Quaterniond();
    double steeringAngleDegrees = 0;
    double pathHysteresis = 0.5;
    double minRadius = 10;
    bool forceCurrentPath = false;

    GuidanceTurning* widget = nullptr;
    KDDockWidgets::DockWidget* dock = nullptr;

  private:
    void calculateTurning( bool changeExistingTurn );

    Plan globalPlan;
    Plan plan;

    Plan::PrimitiveSharedPointer lastPrimitive = nullptr;

    bool turningLeft = false;
    bool turningRight = false;
    int leftSkip = 1;
    int rightSkip = 1;
    Point_2 positionTurnStart = Point_2( 0, 0 );
    double headingTurnStart = 0;
};

class LocalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    LocalPlannerFactory( MyMainWindow* mainWindow,
                         KDDockWidgets::Location location,
                         QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Local Planner" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Guidance" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
};
