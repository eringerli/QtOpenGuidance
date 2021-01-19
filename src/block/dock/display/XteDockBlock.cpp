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
#include <QMenu>
#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

#include "gui/dock/XteDock.h"
#include "gui/MyMainWindow.h"

KDDockWidgets::DockWidget* XteDockBlockFactory::firstDock = nullptr;

XteDockBlock::XteDockBlock( const QString& uniqueName, MyMainWindow* mainWindow ) {
  widget = new XteDock( mainWindow );
  dock = new KDDockWidgets::DockWidget( uniqueName );
}

XteDockBlock::~XteDockBlock() {
  widget->deleteLater();
  dock->deleteLater();

  if( XteDockBlockFactory::firstDock == dock ) {
    XteDockBlockFactory::firstDock = nullptr;
  }
}

void XteDockBlock::setName( const QString& name ) {
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "XTE: " ) + name );
  widget->setName( name );
}

void XteDockBlock::setXte( const double xte ) {
  widget->setXte( xte );
}

QNEBlock* XteDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* object = new XteDockBlock( getNameOfFactory() + QString::number( id ),
                                   mainWindow );
  auto* b = createBaseBlock( scene, object, id );

  object->dock->setTitle( getNameOfFactory() );
  object->dock->setWidget( object->widget );

  menu->addAction( object->dock->toggleAction() );

  if( firstDock == nullptr ) {
    mainWindow->addDockWidget( object->dock, location );
    firstDock = object->dock;
  } else {
    mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnRight, firstDock );
  }

  b->addInputPort( QStringLiteral( "XTE" ), QLatin1String( SLOT( setXte( const double ) ) ) );

  b->setBrush( dockColor );

  return b;
}
