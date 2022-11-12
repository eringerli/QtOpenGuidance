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

#include "ActionDockBlock.h"

#include <QObject>

#include <QAction>
#include <QBrush>
#include <QMenu>

#include <QJsonDocument>
#include <QJsonObject>

#include "gui/MyMainWindow.h"
#include "gui/dock/ActionDock.h"

#include "block/BlockBase.h"
#include "block/dock/display/ValueDockBlockBase.h"

#include "qneblock.h"
#include "qneport.h"

KDDockWidgets::DockWidget* ActionDockBlockFactory::firstActionDock = nullptr;

ActionDockBlock::ActionDockBlock( const QString& uniqueName, MyMainWindow* mainWindow ) {
  widget = new ActionDock( mainWindow );
  dock   = new KDDockWidgets::DockWidget( uniqueName );

  QObject::connect( widget, &ActionDock::action, this, &ActionDockBlock::action );
}

ActionDockBlock::~ActionDockBlock() {
  widget->deleteLater();
  dock->deleteLater();

  if( ActionDockBlockFactory::firstActionDock == dock ) {
    ActionDockBlockFactory::firstActionDock = nullptr;
  }
}

void
ActionDockBlock::setName( const QString& name ) {
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "Action: " ) + name );
}

void
ActionDockBlock::setCheckable( const bool checkable ) {
  widget->setCheckable( checkable );
}

void
ActionDockBlock::setTheme( const QString& theme ) {
  widget->setTheme( theme );
}

QNEBlock*
ActionDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* object = new ActionDockBlock( getNameOfFactory() + QString::number( id ), mainWindow );
  auto* b      = createBaseBlock( scene, object, id );
  object->moveToThread( thread );
  addCompressedObject( object );

  object->dock->setTitle( getNameOfFactory() );
  object->dock->setWidget( object->widget );

  menu->addAction( object->dock->toggleAction() );

  if( firstActionDock == nullptr ) {
    mainWindow->addDockWidget( object->dock, location );
    firstActionDock = object->dock;
  } else {
    mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnBottom, firstActionDock );
  }

  b->addOutputPort( QStringLiteral( "Action with State" ), QLatin1String( SIGNAL( action( ACTION_SIGNATURE ) ) ) );

  addCompressedSignal( QMetaMethod::fromSignal( &ActionDockBlock::action ) );

  b->setBrush( inputDockColor );

  return b;
}
