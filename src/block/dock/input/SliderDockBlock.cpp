// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SliderDockBlock.h"
#include "gui/MyMainWindow.h"

#include <QObject>

#include <QAction>

#include <QMenu>

#include <QJsonDocument>
#include <QJsonObject>

#include "gui/MyMainWindow.h"
#include "gui/dock/SliderDock.h"

#include "block/BlockBase.h"
#include "block/dock/display/ValueDockBlockBase.h"

KDDockWidgets::QtWidgets::DockWidget* SliderDockBlockFactory::firstSliderValueDock = nullptr;

SliderDockBlock::SliderDockBlock(
  MyMainWindow* mainWindow, const QString& uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ) {
  widget = new SliderDock( mainWindow );
  dock   = new KDDockWidgets::QtWidgets::DockWidget( uniqueName );

  connect( widget, &SliderDock::valueChanged, this, &SliderDockBlock::valueChangedProxy );
}

SliderDockBlock::~SliderDockBlock() {
  dock->deleteLater();
  widget->deleteLater();

  if( SliderDockBlockFactory::firstSliderValueDock == dock ) {
    SliderDockBlockFactory::firstSliderValueDock = nullptr;
  }
}

void
SliderDockBlock::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT valueChanged( widget->getValue(), CalculationOption::Option::None );
}

void
SliderDockBlock::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "Value" )]          = widget->getValue();
  valuesObject[QStringLiteral( "Decimals" )]       = widget->getDecimals();
  valuesObject[QStringLiteral( "Maximum" )]        = widget->getMaximum();
  valuesObject[QStringLiteral( "Minimum" )]        = widget->getMinimum();
  valuesObject[QStringLiteral( "DefaultValue" )]   = widget->getDefaultValue();
  valuesObject[QStringLiteral( "Unit" )]           = widget->getUnit();
  valuesObject[QStringLiteral( "ResetOnStart" )]   = widget->resetOnStart;
  valuesObject[QStringLiteral( "SliderInverted" )] = widget->getSliderInverted();
}

void
SliderDockBlock::fromJSON( const QJsonObject& valuesObject ) {
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

void
SliderDockBlock::setName( const QString& name ) {
  BlockBase::setName( name );
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "Value: " ) + name );
}

void
SliderDockBlock::setValue( double value ) {
  widget->setValue( value );
}

void
SliderDockBlock::valueChangedProxy( double value ) {
  Q_EMIT valueChanged( value, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
SliderDockBlockFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< SliderDockBlock >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( firstSliderValueDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    firstSliderValueDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, firstSliderValueDock );
  }

  obj->addOutputPort( QStringLiteral( "Number" ), obj.get(), QLatin1StringView( SIGNAL( valueChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
