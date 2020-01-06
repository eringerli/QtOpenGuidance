// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef FPSMEASUREMENT_H
#define FPSMEASUREMENT_H

#include <QObject>

#include <Qt3DCore/QEntity>

#include "BlockBase.h"

#include "../aspect/fpsmonitor.h"

class FpsMeasurement : public BlockBase {
    Q_OBJECT

  public:
    explicit FpsMeasurement( Qt3DCore::QEntity* rootEntity )
      : BlockBase() {

      fpsEntity = new Qt3DCore::QEntity( rootEntity );
      fpsComponent = new FpsMonitor( fpsEntity );
      fpsEntity->addComponent( fpsComponent );
      fpsComponent->setRollingMeanFrameCount( 20 );
      QObject::connect( fpsComponent,
                        &FpsMonitor::framesPerSecondChanged,
                        this,
                        &FpsMeasurement::fpsChanged );
    }

    void emitConfigSignals() override {
      emit fpsChanged( 0 );
    }

  signals:
    void fpsChanged( float );

  public:
    Qt3DCore::QEntity* fpsEntity = nullptr;
    FpsMonitor* fpsComponent = nullptr;
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

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new FpsMeasurement( rootEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj, true );

      b->addOutputPort( QStringLiteral( "FPS" ), QLatin1String( SIGNAL( fpsChanged( float ) ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};

#endif // FPSMEASUREMENT_H
