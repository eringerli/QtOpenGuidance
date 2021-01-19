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

#include "3d/qt3dForwards.h"

#include "block/BlockBase.h"

class FpsMeasurement : public BlockBase {
    Q_OBJECT

  public:
    explicit FpsMeasurement( Qt3DCore::QEntity* rootEntity );
    ~FpsMeasurement();

    void emitConfigSignals() override;

  public Q_SLOTS:
    void frameActionTriggered( const float dt );

  Q_SIGNALS:
    void fpsChanged( const double );

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

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};
