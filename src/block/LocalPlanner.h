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

#include <QVector>
#include <QSharedPointer>

class LocalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit LocalPlanner(const QString& uniqueName,
                          MyMainWindow* mainWindow)
      : BlockBase() {
      widget = new GuidanceTurning( mainWindow );
      dock = new KDDockWidgets::DockWidget( uniqueName );
    }

    ~LocalPlanner(){
      dock->deleteLater();
      widget->deleteLater();
    }

  public slots:
    void setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options );

    void setPlan( const Plan& plan ) {
      this->globalPlan = plan;
//      emit planChanged( plan );
    }


  signals:
    void planChanged( const Plan& );

  public:
    Point_3 position = Point_3( 0, 0, 0 );
    QQuaternion orientation = QQuaternion();

    GuidanceTurning* widget = nullptr;
KDDockWidgets::DockWidget* dock = nullptr;

  private:
    Plan globalPlan;
    Plan plan;
};

class LocalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    LocalPlannerFactory(MyMainWindow* mainWindow,
                        KDDockWidgets::Location location,
                        QMenu* menu)
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Local Planner" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* object = new LocalPlanner(getNameOfFactory() + QString::number( id ),
                                   mainWindow);
      auto* b = createBaseBlock( scene, object, id );

      object->dock->setTitle( getNameOfFactory() );
      object->dock->setWidget( object->widget );

      menu->addAction( object->dock->toggleAction() );

      mainWindow->addDockWidget( object->dock, location );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );
      b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( const Plan& ) ) ) );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
};
