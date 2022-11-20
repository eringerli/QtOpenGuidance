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
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) ) {
    widget->setXte( xte );
  }
}

const QFont&
XteDockBlock::getFont() {
  return widget->fontOfLabel();
}

int
XteDockBlock::getPrecision() {
  return widget->precision;
}

int
XteDockBlock::getFieldWidth() {
  return widget->fieldWidth;
}

double
XteDockBlock::getScale() {
  return widget->scale;
}

bool
XteDockBlock::unitVisible() {
  return widget->unitEnabled;
}

const QString&
XteDockBlock::getUnit() {
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
  addCompressedObject( obj );

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

  addCompressedSignal( QMetaMethod::fromSignal( &XteDockBlock::setXte ) );

  b->setBrush( dockColor );

  return b;
}
