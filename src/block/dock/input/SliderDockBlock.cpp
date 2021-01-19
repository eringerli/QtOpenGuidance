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

#include "SliderDockBlock.h"
#include "gui/MyMainWindow.h"

#include <QObject>

#include <QMenu>
#include <QAction>
#include <QBrush>

#include <QJsonDocument>
#include <QJsonObject>

#include "gui/MyMainWindow.h"
#include "gui/dock/SliderDock.h"

#include "block/BlockBase.h"
#include "block/dock/display/ValueDockBlockBase.h"

#include "qneblock.h"
#include "qneport.h"

KDDockWidgets::DockWidget* SliderDockBlockFactory::firstSliderValueDock = nullptr;

SliderDockBlock::SliderDockBlock( const QString& uniqueName, MyMainWindow* mainWindow )
  : BlockBase() {
  widget = new SliderDock( mainWindow );
  dock = new KDDockWidgets::DockWidget( uniqueName );

  connect( widget, &SliderDock::valueChanged, this, &SliderDockBlock::valueChanged );
}

SliderDockBlock::~SliderDockBlock() {
  dock->deleteLater();
  widget->deleteLater();

  if( SliderDockBlockFactory::firstSliderValueDock == dock ) {
    SliderDockBlockFactory::firstSliderValueDock = nullptr;
  }
}

void SliderDockBlock::emitConfigSignals() {
  Q_EMIT valueChanged( widget->getValue() );
}

void SliderDockBlock::toJSON( QJsonObject& json ) {
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

void SliderDockBlock::fromJSON( QJsonObject& json ) {
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

void SliderDockBlock::setName( const QString& name ) {
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "Value: " ) + name );
}

void SliderDockBlock::setValue( const double value ) {
  widget->setValue( value );
}

QNEBlock* SliderDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
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

  b->addOutputPort( QStringLiteral( "Number" ), QLatin1String( SIGNAL( valueChanged( const double ) ) ) );

  b->setBrush( inputDockColor );

  return b;
}
