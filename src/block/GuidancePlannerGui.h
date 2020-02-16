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

#ifndef PLANNERGUI_H
#define PLANNERGUI_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"

class PlannerGui : public BlockBase {
    Q_OBJECT

  public:
    explicit PlannerGui( Qt3DCore::QEntity* rootEntity )
      : BlockBase(),
        rootEntity( rootEntity ) {}

  public slots:
    void setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation = orientation;
      }
    }

  signals:
    void a_clicked();
    void b_clicked();
    void snap_clicked();
    void autosteerEnabled( bool );
    void turnLeft_clicked();
    void turnRight_clicked();
    void xteChanged( double );

  public:
    Point_3 position = Point_3();
    QQuaternion orientation = QQuaternion();

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};

class PlannerGuiFactory : public BlockFactory {
    Q_OBJECT

  public:
    PlannerGuiFactory( Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Planner GUI" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene ) override {
      auto* obj = new PlannerGui( rootEntity );
      auto* b = createBaseBlock( scene, obj, true );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "A clicked" ), QLatin1String( SIGNAL( a_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "B clicked" ), QLatin1String( SIGNAL( b_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "Snap clicked" ), QLatin1String( SIGNAL( snap_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "Turn Left" ), QLatin1String( SIGNAL( turnLeft_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "Turn Right" ), QLatin1String( SIGNAL( turnRight_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "Autosteer Enabled" ), QLatin1String( SIGNAL( autosteerEnabled( bool ) ) ) );
      b->addInputPort( QStringLiteral( "XTE" ), QLatin1String( SIGNAL( xteChanged( double ) ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};

#endif // PLANNERGUI_H

