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

#include "ValueDockBlock.h"

#include "gui/MyMainWindow.h"
#include "gui/dock/ValueDock.h"

#include <QAction>
#include <QBrush>
#include <QMenu>

#include "qneblock.h"
#include "qneport.h"

ValueDockBlock::ValueDockBlock( const QString& uniqueName, MyMainWindow* mainWindow ) : ValueDockBlockBase( uniqueName ) {
  widget = new ValueDock( mainWindow );
}

ValueDockBlock::~ValueDockBlock() {
  widget->deleteLater();

  if( ValueDockBlockBase::firstValueDock == dock ) {
    ValueDockBlockBase::firstValueDock = nullptr;
  }
}

const QFont&
ValueDockBlock::getFont() {
  return widget->fontOfLabel();
}

int
ValueDockBlock::getPrecision() {
  return widget->precision;
}

int
ValueDockBlock::getFieldWidth() {
  return widget->fieldWidth;
}

double
ValueDockBlock::getScale() {
  return widget->scale;
}

bool
ValueDockBlock::unitVisible() {
  return widget->unitEnabled;
}

const QString&
ValueDockBlock::getUnit() {
  return widget->unit;
}

void
ValueDockBlock::setFont( const QFont& font ) {
  widget->setFontOfLabel( font );
}

void
ValueDockBlock::setPrecision( const int precision ) {
  widget->precision = precision;
}

void
ValueDockBlock::setFieldWidth( const int fieldWidth ) {
  widget->fieldWidth = fieldWidth;
}

void
ValueDockBlock::setScale( const double scale ) {
  widget->scale = scale;
}

void
ValueDockBlock::setUnitVisible( const bool enabled ) {
  widget->unitEnabled = enabled;
}

void
ValueDockBlock::setUnit( const QString& unit ) {
  widget->unit = unit;
}

void
ValueDockBlock::setName( const QString& name ) {
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "Value: " ) + name );
}

void
ValueDockBlock::setValue( const double value, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) ) {
    widget->setMeter( value );
  }
}

QNEBlock*
ValueDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* object = new ValueDockBlock( getNameOfFactory() + QString::number( id ), mainWindow );
  auto* b      = createBaseBlock( scene, object, id );
  object->moveToThread( thread );
  addCompressedObject( object );

  object->dock->setTitle( getNameOfFactory() );
  object->dock->setWidget( object->widget );

  menu->addAction( object->dock->toggleAction() );

  if( ValueDockBlockBase::firstValueDock == nullptr ) {
    mainWindow->addDockWidget( object->dock, location );
    ValueDockBlockBase::firstValueDock = object->dock;
  } else {
    mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnBottom, ValueDockBlockBase::firstValueDock );
  }

  b->addInputPort( QStringLiteral( "Number" ), QLatin1String( SLOT( setValue( NUMBER_SIGNATURE ) ) ) );

  addCompressedSignal( QMetaMethod::fromSignal( &ValueDockBlock::setValue ) );

  b->setBrush( dockColor );

  return b;
}
