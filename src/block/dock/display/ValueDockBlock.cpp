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

#include <QMenu>

ValueDockBlock::ValueDockBlock(
  MyMainWindow* mainWindow, const QString& uniqueName, const int idHint, const bool systemBlock, const QString type )
    : ValueDockBlockBase( uniqueName, idHint, systemBlock, type ) {
  widget = new ValueDock( mainWindow );
}

ValueDockBlock::~ValueDockBlock() {
  widget->deleteLater();

  if( ValueDockBlockBase::firstValueDock == dock ) {
    ValueDockBlockBase::firstValueDock = nullptr;
  }
}

const QFont&
ValueDockBlock::getFont() const {
  return widget->fontOfLabel();
}

int
ValueDockBlock::getPrecision() const {
  return widget->precision;
}

int
ValueDockBlock::getFieldWidth() const {
  return widget->fieldWidth;
}

double
ValueDockBlock::getScale() const {
  return widget->scale;
}

bool
ValueDockBlock::unitVisible() const {
  return widget->unitEnabled;
}

const QString&
ValueDockBlock::getUnit() const {
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
  BlockBase::setName( name );
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "Value: " ) + name );
}

void
ValueDockBlock::setValue( const double value, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::ValueDock ) ) {
    widget->setMeter( value );
  }
}

std::unique_ptr< BlockBase >
ValueDockBlockFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ValueDockBlock >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( ValueDockBlockBase::firstValueDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    ValueDockBlockBase::firstValueDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, ValueDockBlockBase::firstValueDock );
  }

  obj->addInputPort( QStringLiteral( "Number" ), obj.get(), QLatin1StringView( SLOT( setValue( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
