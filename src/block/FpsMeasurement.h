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

#include "../gui/VectorBlockModel.h"

#pragma once

#include <QObject>

#include <Qt3DCore/QEntity>
#include <Qt3DLogic/QFrameAction>

#include "BlockBase.h"

class FpsMeasurement : public BlockBase {
    Q_OBJECT

  public:
    explicit FpsMeasurement( Qt3DCore::QEntity* rootEntity )
      : BlockBase() {
      fpsComponent = new Qt3DLogic::QFrameAction( rootEntity );
      rootEntity->addComponent( fpsComponent );
      QObject::connect( fpsComponent, &Qt3DLogic::QFrameAction::triggered, this, &FpsMeasurement::frameActionTriggered );
    }

    ~FpsMeasurement() {
      fpsComponent->deleteLater();
    }

    void emitConfigSignals() override {
      emit fpsChanged( 0 );
    }

  public slots:
    void frameActionTriggered( float dt ) {
      emit fpsChanged( 1 / dt );
    }

  signals:
    void fpsChanged( double );

  public:
    Qt3DLogic::QFrameAction* fpsComponent;
};



class FpsMeasurementFactory : public BlockFactory {
    Q_OBJECT

  public:
    FpsMeasurementFactory( Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Fps Measurement" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Graphical" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new FpsMeasurement( rootEntity );
      auto* b = createBaseBlock( scene, obj, id, true );

      b->addOutputPort( QStringLiteral( "FPS" ), QLatin1String( SIGNAL( fpsChanged( double ) ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};
