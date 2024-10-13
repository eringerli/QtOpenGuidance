// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XteDockBlock.h"

#include <QAction>

#include <QMenu>

#include "gui/MyMainWindow.h"
#include "gui/dock/XteDock.h"

KDDockWidgets::QtWidgets::DockWidget* XteDockBlockFactory::firstDock = nullptr;

XteDockBlock::XteDockBlock(
  MyMainWindow* mainWindow, const QString& uniqueName, const int idHint, const bool systemBlock, const QString type )
    : ValueDockBlockBase( uniqueName, idHint, systemBlock, type ) {
  widget = new XteDock( mainWindow );
}

XteDockBlock::~XteDockBlock() {
  widget->deleteLater();

  if( XteDockBlockFactory::firstDock == dock ) {
    XteDockBlockFactory::firstDock = nullptr;
  }
}

void
XteDockBlock::setName( const QString& name ) {
  BlockBase::setName( name );
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "XTE: " ) + name );
  widget->setName( name );
}

void
XteDockBlock::setXte( const double xte, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::ValueDock ) ) {
    widget->setXte( xte );
  }
}

const QFont&
XteDockBlock::getFont() const {
  return widget->fontOfLabel();
}

int
XteDockBlock::getPrecision() const {
  return widget->precision;
}

int
XteDockBlock::getFieldWidth() const {
  return widget->fieldWidth;
}

double
XteDockBlock::getScale() const {
  return widget->scale;
}

bool
XteDockBlock::unitVisible() const {
  return widget->unitEnabled;
}

const QString&
XteDockBlock::getUnit() const {
  return widget->unit;
}

void
XteDockBlock::setFont( const QFont& font ) {
  widget->setFontOfLabel( font );
}

void
XteDockBlock::setPrecision( const int precision ) {
  widget->precision = precision;
}

void
XteDockBlock::setFieldWidth( const int fieldWidth ) {
  widget->fieldWidth = fieldWidth;
}

void
XteDockBlock::setScale( const double scale ) {
  widget->scale = scale;
}

void
XteDockBlock::setUnitVisible( const bool enabled ) {
  widget->unitEnabled = enabled;
}

void
XteDockBlock::setUnit( const QString& unit ) {
  widget->unit = unit;
}

std::unique_ptr< BlockBase >
XteDockBlockFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< XteDockBlock >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( firstDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    firstDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnRight, firstDock );
  }

  obj->addInputPort( QStringLiteral( "XTE" ), obj.get(), QLatin1StringView( SLOT( setXte( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
