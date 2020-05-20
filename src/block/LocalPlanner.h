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

#include "../gui/GuidanceTurning.h"

#include "../gui/MyMainWindow.h"
#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>


#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>
#include "../3d/BufferMesh.h"

class LocalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit LocalPlanner( const QString& uniqueName,
                           MyMainWindow* mainWindow
                         )
      : BlockBase() {
      widget = new GuidanceTurning( mainWindow );
      dock = new KDDockWidgets::DockWidget( uniqueName );

      QObject::connect( widget, &GuidanceTurning::turnLeftToggled, this, &LocalPlanner::turnLeftToggled );
      QObject::connect( widget, &GuidanceTurning::turnRightToggled, this, &LocalPlanner::turnRightToggled );
      QObject::connect( widget, &GuidanceTurning::numSkipChanged, this, &LocalPlanner::numSkipChanged );
      QObject::connect( this, &LocalPlanner::resetTurningStateOfDock, widget, &GuidanceTurning::resetTurningState );
    }

    ~LocalPlanner() {
      dock->deleteLater();
      widget->deleteLater();
    }

  public slots:
    void setName( const QString& name ) override {
      dock->setTitle( name );
      dock->toggleAction()->setText( QStringLiteral( "Turning Dock: " ) + name );
    }

    void setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options );

    void setPlan( const Plan& plan );

    void setSteeringAngle( double steeringAngle ) {
      this->steeringAngleDegrees = steeringAngle;
    }

    void setPathHysteresis( const double pathHysteresis ) {
      this->pathHysteresis = pathHysteresis;
    }

    void setMinRadius( const double minRadius ) {
      this->minRadius = minRadius;
    }

    void turnLeftToggled( bool state );
    void turnRightToggled( bool state );
    void numSkipChanged( int left, int right );

  signals:
    void planChanged( const Plan& );
    void triggerPlanPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options );
    void resetTurningStateOfDock();

  public:
    Point_3 position = Point_3( 0, 0, 0 );
    QQuaternion orientation = QQuaternion();
    double steeringAngleDegrees = 0;
    double pathHysteresis = 0.5;
    double minRadius = 10;

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
    Plan::ConstPrimitiveIterator lastSegmentOfTurn;
    Plan::ConstPrimitiveIterator targetSegmentOfTurn;
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

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* object = new LocalPlanner( getNameOfFactory() + QString::number( id ),
                                       mainWindow );
      auto* b = createBaseBlock( scene, object, id );

      object->dock->setTitle( getNameOfFactory() );
      object->dock->setWidget( object->widget );

      menu->addAction( object->dock->toggleAction() );

      mainWindow->addDockWidget( object->dock, location );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( double ) ) ) );
      b->addInputPort( QStringLiteral( "Path Hysteresis" ), QLatin1String( SLOT( setPathHysteresis( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Minimum Radius" ), QLatin1String( SLOT( setMinRadius( const double ) ) ) );

      b->addOutputPort( QStringLiteral( "Trigger Plan Pose" ), QLatin1String( SIGNAL( triggerPlanPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( const Plan& ) ) ) );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
};
