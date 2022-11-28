// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
  auto* obj = new ActionDockBlock( getNameOfFactory() + QString::number( id ), mainWindow );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( firstActionDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    firstActionDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, firstActionDock );
  }

  b->addOutputPort( QStringLiteral( "Action with State" ), QLatin1String( SIGNAL( action( ACTION_SIGNATURE ) ) ) );

  b->setBrush( inputDockColor );

  return b;
}
