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
#include <QDockWidget>
#include <QSizePolicy>
#include <QMenu>

#include "../gui/MyMainWindow.h"
#include "../gui/SliderDock.h"

#include "BlockBase.h"
#include "ValueDockBlockBase.h"

class SliderDockBlock : public BlockBase {
    Q_OBJECT

  public:
    explicit SliderDockBlock( const QString& uniqueName,
                              MyMainWindow* mainWindow )
      : BlockBase() {
      widget = new SliderDock( mainWindow );
      dock = new KDDockWidgets::DockWidget( uniqueName );

      connect( widget, &SliderDock::valueChanged, this, &SliderDockBlock::valueChanged );
    }

    ~SliderDockBlock();

    virtual void emitConfigSignals() override {
      emit valueChanged( widget->getValue() );
    }

    virtual void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;
      valuesObject[QStringLiteral( "Value" )] = widget->getValue();
      valuesObject[QStringLiteral( "Decimals" )] = widget->getDecimals();
      valuesObject[QStringLiteral( "Maximum" )] = widget->getMaximum();
      valuesObject[QStringLiteral( "Minimum" )] = widget->getMinimum();
      valuesObject[QStringLiteral( "DefaultValue" )] = widget->getDefaultValue();
      valuesObject[QStringLiteral( "Unit" )] = widget->getUnit();
      valuesObject[QStringLiteral( "ResetOnStart" )] = widget->resetOnStart;
      valuesObject[QStringLiteral( "SliderInverted" )] = widget->getSliderInverted();

      json[QStringLiteral( "values" )] = valuesObject;
    }

    virtual void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        if( valuesObject[QStringLiteral( "ResetOnStart" )].isBool() ) {
          widget->resetOnStart = valuesObject[QStringLiteral( "ResetOnStart" )].toBool();
        }

        if( valuesObject[QStringLiteral( "Decimals" )].isDouble() ) {
          widget->setDecimals( valuesObject[QStringLiteral( "Decimals" )].toDouble() );
        }

        if( valuesObject[QStringLiteral( "Maximum" )].isDouble() ) {
          widget->setMaximum( valuesObject[QStringLiteral( "Maximum" )].toDouble() );
        }

        if( valuesObject[QStringLiteral( "Minimum" )].isDouble() ) {
          widget->setMinimum( valuesObject[QStringLiteral( "Minimum" )].toDouble() );
        }

        if( valuesObject[QStringLiteral( "DefaultValue" )].isDouble() ) {
          widget->setDefaultValue( valuesObject[QStringLiteral( "DefaultValue" )].toDouble() );
        }

        if( valuesObject[QStringLiteral( "Unit" )].isDouble() ) {
          widget->setUnit( valuesObject[QStringLiteral( "Unit" )].toString() );
        }

        if( valuesObject[QStringLiteral( "SliderInverted" )].isBool() ) {
          widget->setSliderInverted( valuesObject[QStringLiteral( "SliderInverted" )].toBool() );
        }

        if( widget->resetOnStart ) {
          widget->setValue( widget->getDefaultValue() );
        } else {
          if( valuesObject[QStringLiteral( "Value" )].isDouble() ) {
            widget->setValue( valuesObject[QStringLiteral( "Value" )].toDouble() );
          }
        }

      }
    }

  public slots:
    void setName( const QString& name ) override {
      dock->setTitle( name );
      dock->toggleAction()->setText( QStringLiteral( "Value: " ) + name );
    }

    void setValue( double value ) {
      widget->setValue( value );
    }

  signals:
    void valueChanged( double );

  public:
    SliderDock* widget = nullptr;

    KDDockWidgets::DockWidget* dock = nullptr;
};

class SliderDockBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    SliderDockBlockFactory( MyMainWindow* mainWindow,
                            KDDockWidgets::Location location,
                            QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "SliderDockBlock" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      if( id != 0 && !isIdUnique( scene, id ) ) {
        id = QNEBlock::getNextUserId();
      }

      auto* object = new SliderDockBlock( getNameOfFactory() + QString::number( id ),
                                          mainWindow );
      auto* b = createBaseBlock( scene, object, id );

      object->dock->setTitle( getNameOfFactory() );
      object->dock->setWidget( object->widget );

      menu->addAction( object->dock->toggleAction() );

      if( firstSliderValueDock == nullptr ) {
        mainWindow->addDockWidget( object->dock, location );
        firstSliderValueDock = object->dock;
      } else {
        mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnBottom, firstSliderValueDock );
      }

      b->addOutputPort( QStringLiteral( "Number" ), QLatin1String( SIGNAL( valueChanged( double ) ) ) );

      b->setBrush( QColor( QStringLiteral( "gold" ) ) );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;

  public:
    static KDDockWidgets::DockWidget* firstSliderValueDock;

};
