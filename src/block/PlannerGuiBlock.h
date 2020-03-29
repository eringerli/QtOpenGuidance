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

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"

class PlannerGuiBlock : public BlockBase {
    Q_OBJECT

  public:
    explicit PlannerGuiBlock()
      : BlockBase() {}

  public slots:

  signals:
    void a_clicked();
    void b_clicked();
    void snap_clicked();
    void autosteerEnabled( bool );
    void turnLeft_clicked();
    void turnRight_clicked();
};

class PlannerGuiBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    PlannerGuiBlockFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Planner GUI Block" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new PlannerGuiBlock();
      auto* b = createBaseBlock( scene, obj, id, true );

      b->addOutputPort( QStringLiteral( "A clicked" ), QLatin1String( SIGNAL( a_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "B clicked" ), QLatin1String( SIGNAL( b_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "Snap clicked" ), QLatin1String( SIGNAL( snap_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "Turn Left" ), QLatin1String( SIGNAL( turnLeft_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "Turn Right" ), QLatin1String( SIGNAL( turnRight_clicked() ) ) );
      b->addOutputPort( QStringLiteral( "Autosteer Enabled" ), QLatin1String( SIGNAL( autosteerEnabled( bool ) ) ) );

      return b;
    }
};
