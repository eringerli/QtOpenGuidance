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
#include <QSizePolicy>
#include <QMenu>

#include "../gui/MainWindow.h"
#include "../gui/GuidanceMeterBar.h"

#include "BlockBase.h"

class MeterBarModel : public BlockBase {
    Q_OBJECT

  public:
    explicit MeterBarModel( MainWindow* mainWindow )
      : BlockBase() {
      widget = new GuidanceMeterBar( mainWindow );
      dock = new QDockWidget( mainWindow );
      dock->setWidget( widget );
    }

    ~MeterBarModel() {
      widget->deleteLater();
      dock->deleteLater();
    }


  public slots:
    void setName( QString name ) override {
      dock->setWindowTitle( name );
      action->setText( QStringLiteral( "Meter: " ) + name );
      widget->setName( name );
    }

    void setMeter( float meter ) {
      widget->setMeter( meter );
    }

  public:

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;

      valuesObject["Font"] = QJsonValue::fromVariant( QVariant( widget->fontOfLabel() ) );
      valuesObject["Precision"] = widget->precision;
      valuesObject["Scale"] = widget->scale;
      valuesObject["FieldWitdh"] = widget->fieldWidth;

      json["values"] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json["values"].isObject() ) {
        QJsonObject valuesObject = json["values"].toObject();

        widget->setFontOfLabel( valuesObject["Font"].toVariant().value<QFont>() );
        widget->precision = valuesObject["Precision"].toInt();
        widget->scale = valuesObject["Scale"].toDouble();
        widget->fieldWidth = valuesObject["FieldWitdh"].toInt();
      }
    }

    QDockWidget* dock = nullptr;
    QAction* action = nullptr;
    GuidanceMeterBar* widget = nullptr;
};

class MeterBarModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    MeterBarModelFactory( MainWindow* mainWindow,
                          Qt::DockWidgetArea area,
                          Qt::DockWidgetAreas allowedAreas,
                          QDockWidget::DockWidgetFeatures features,
                          QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        area( area ),
        allowedAreas( allowedAreas ),
        features( features ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "MeterBarModel" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new MeterBarModel( mainWindow );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      MeterBarModel* object = qobject_cast<MeterBarModel*>( obj );

      object->dock->setWidget( object->widget );
      object->dock->setFeatures( features );
      object->dock->setAllowedAreas( allowedAreas );
      object->dock->setObjectName( getNameOfFactory() + QString::number( b->id ) );

      object->action = object->dock->toggleViewAction();
      menu->addAction( object->action );

      mainWindow->addDockWidget( area, object->dock );

      b->addInputPort( "Number", SLOT( setMeter( float ) ) );

      return b;
    }

  private:
    MainWindow* mainWindow = nullptr;
    Qt::DockWidgetArea area;
    Qt::DockWidgetAreas allowedAreas;
    QDockWidget::DockWidgetFeatures features;
    QMenu* menu = nullptr;
};

#endif // METERBARMODEL_H
