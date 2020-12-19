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

#include "qneblock.h"
#include "qneport.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

class PoseSynchroniser : public BlockBase {
    Q_OBJECT

  public:
    explicit PoseSynchroniser()
      : BlockBase() {}

  public Q_SLOTS:
    void setPosition( const Eigen::Vector3d& position ) {
      this->position = position;
//      QElapsedTimer timer;
//      timer.start();
      Q_EMIT poseChanged( this->position, orientation, PoseOption::NoOptions );
//      qDebug() << "Cycle Time PoseSynchroniser:  " << timer.nsecsElapsed() << "ns";
    }

    void setOrientation( const Eigen::Quaterniond& value ) {
      orientation = value;
    }

  Q_SIGNALS:
    void poseChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );

  public:
    virtual void emitConfigSignals() override {
      Q_EMIT poseChanged( position, orientation, PoseOption::NoOptions );
    }

  public:
    Eigen::Vector3d position = Eigen::Vector3d( 0, 0, 0 );
    Eigen::Quaterniond orientation = Eigen::Quaterniond( 0, 0, 0, 0 );
};

class PoseSynchroniserFactory : public BlockFactory {
    Q_OBJECT

  public:
    PoseSynchroniserFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Pose Synchroniser" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Guidance" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new PoseSynchroniser();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Position" ), QLatin1String( SLOT( setPosition( const Eigen::Vector3d& ) ) ) );
      b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( const Eigen::Quaterniond& ) ) ) );

      b->addOutputPort( QStringLiteral( "Pose" ), QLatin1String( SIGNAL( poseChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

      return b;
    }
};
