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

#include "gui/MyMainWindow.h"
#include "gui/dock/ValueDock.h"

#include "block/BlockBase.h"
#include "ValueDockBlockBase.h"

class ValueDockBlock : public ValueDockBlockBase {
    Q_OBJECT

  public:
    explicit ValueDockBlock( const QString& uniqueName,
                             MyMainWindow* mainWindow )
      : ValueDockBlockBase( uniqueName ) {
      widget = new ValueDock( mainWindow );
    }

    ~ValueDockBlock() {
      widget->deleteLater();

      if( ValueDockBlockBase::firstValueDock == dock ) {
        ValueDockBlockBase::firstValueDock = nullptr;
      }
    }

    virtual const QFont& getFont() override {
      return widget->fontOfLabel();
    }
    virtual int getPrecision() override {
      return widget->precision;
    }
    virtual int getFieldWidth() override {
      return widget->fieldWidth;
    }
    virtual double getScale() override {
      return widget->scale;
    }
    virtual bool unitVisible() override {
      return widget->unitEnabled;
    }
    virtual const QString& getUnit() override {
      return widget->unit;
    }

    virtual void setFont( const QFont& font ) override {
      widget->setFontOfLabel( font );
    }
    virtual void setPrecision( const int precision ) override {
      widget->precision = precision;
    }
    virtual void setFieldWidth( const int fieldWidth ) override {
      widget->fieldWidth = fieldWidth;
    }
    virtual void setScale( const double scale ) override {
      widget->scale = scale;
    }
    virtual void setUnitVisible( const bool enabled ) override {
      widget->unitEnabled = enabled;
    }
    virtual void setUnit( const QString& unit ) override {
      widget->unit = unit;
    }

  public Q_SLOTS:
    void setName( const QString& name ) override {
      dock->setTitle( name );
      dock->toggleAction()->setText( QStringLiteral( "Value: " ) + name );
    }

    void setValue( const double value ) {
      widget->setMeter( value );
    }

  public:
    ValueDock* widget = nullptr;
};

class ValueDockBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    ValueDockBlockFactory( MyMainWindow* mainWindow,
                           KDDockWidgets::Location location,
                           QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "ValueDockBlock" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Display Docks" );
    }

    QString getPrettyNameOfFactory() override {
      return QStringLiteral( "Value Dock" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      if( id != 0 && !isIdUnique( scene, id ) ) {
        id = QNEBlock::getNextUserId();
      }

      auto* object = new ValueDockBlock( getNameOfFactory() + QString::number( id ),
                                         mainWindow );
      auto* b = createBaseBlock( scene, object, id );

      object->dock->setTitle( getNameOfFactory() );
      object->dock->setWidget( object->widget );

      menu->addAction( object->dock->toggleAction() );

      if( ValueDockBlockBase::firstValueDock == nullptr ) {
        mainWindow->addDockWidget( object->dock, location );
        ValueDockBlockBase::firstValueDock = object->dock;
      } else {
        mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnBottom, ValueDockBlockBase::firstValueDock );
      }

      b->addInputPort( QStringLiteral( "Number" ), QLatin1String( SLOT( setValue( const double ) ) ) );

      b->setBrush( dockColor );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
};
