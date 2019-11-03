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

#ifndef METERBARMODEL_H
#define METERBARMODEL_H

#include <QObject>
#include <QDockWidget>

#include "../gui/MainWindow.h"
#include "../gui/GuidanceMeterBar.h"

#include "BlockBase.h"

class MeterBarModel : public BlockBase {
    Q_OBJECT

  public:
    explicit MeterBarModel( MainWindow* mainWindow,
                            Qt::DockWidgetArea area,
                            Qt::DockWidgetAreas allowedAreas,
                            QDockWidget::DockWidgetFeatures features )
      : BlockBase() {
      guidanceMeterBar = new GuidanceMeterBar( mainWindow );
      dock = new QDockWidget( mainWindow );
      dock->setWidget( guidanceMeterBar );
      dock->setFeatures( features );
      dock->setAllowedAreas( allowedAreas );

      mainWindow->addDockWidget( area, dock );
    }

    ~MeterBarModel() {
      guidanceMeterBar->deleteLater();
      dock->deleteLater();
    }


  public slots:
    void setName( QString name ) {
      dock->setWindowTitle( name );
      guidanceMeterBar->setName( name );
    }

    void setMeter( float meter ) {
      guidanceMeterBar->setMeter( meter );
    }

    void setPrecision( float precision ) {
      guidanceMeterBar->setPrecision( precision );
    }

    void setScale( float scale ) {
      guidanceMeterBar->setScale( scale );
    }

    void setFieldWitdh( float fieldWitdh ) {
      guidanceMeterBar->setFieldWitdh( fieldWitdh );
    }

  public:
    QDockWidget* dock = nullptr;
    GuidanceMeterBar* guidanceMeterBar = nullptr;
};

class MeterBarModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    MeterBarModelFactory( MainWindow* mainWindow,
                          Qt::DockWidgetArea area,
                          Qt::DockWidgetAreas allowedAreas,
                          QDockWidget::DockWidgetFeatures features )
      : BlockFactory(),
        mainWindow( mainWindow ),
        area( area ),
        allowedAreas( allowedAreas ),
        features( features ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "MeterBarModel" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new MeterBarModel( mainWindow, area, allowedAreas, features );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( "Number", SLOT( setMeter( float ) ) );
      b->addInputPort( "Precision", SLOT( setPrecision( float ) ) );
      b->addInputPort( "Scale", SLOT( setScale( float ) ) );
      b->addInputPort( "FieldWitdh", SLOT( setFieldWitdh( float ) ) );

      return b;
    }

  private:
    MainWindow* mainWindow = nullptr;
    Qt::DockWidgetArea area;
    Qt::DockWidgetAreas allowedAreas;
    QDockWidget::DockWidgetFeatures features;
};

#endif // METERBARMODEL_H
