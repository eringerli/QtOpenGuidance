// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XteDockBlock.h"

#include <QAction>
#include <QBrush>
#include <QMenu>

#include "qneblock.h"
#include "qneport.h"

#include "gui/MyMainWindow.h"
#include "gui/dock/XteDock.h"

KDDockWidgets::DockWidget* XteDockBlockFactory::firstDock = nullptr;

XteDockBlock::XteDockBlock( const QString& uniqueName, MyMainWindow* mainWindow ) : ValueDockBlockBase( uniqueName ) {
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
XteDockBlock::getPrecision()const{
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

QNEBlock*
XteDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* obj = new XteDockBlock( getNameOfFactory() + QString::number( id ), mainWindow );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( firstDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    firstDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnRight, firstDock );
  }

  b->addInputPort( QStringLiteral( "XTE" ), QLatin1String( SLOT( setXte( NUMBER_SIGNATURE ) ) ) );

  b->setBrush( dockColor );

  return b;
}
