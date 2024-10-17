// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ActionDockBlock.h"

#include <QObject>

#include <QAction>

#include <QMenu>

#include <QJsonDocument>
#include <QJsonObject>

#include "gui/MyMainWindow.h"
#include "gui/dock/ActionDock.h"

#include "block/BlockBase.h"
#include "block/dock/display/ValueDockBlockBase.h"

KDDockWidgets::QtWidgets::DockWidget* ActionDockBlockFactory::firstActionDock = nullptr;

ActionDockBlock::ActionDockBlock( MyMainWindow*              mainWindow,
                                  const QString&             uniqueName,
                                  const BlockBaseId          idHint,
                                  const bool                 systemBlock,
                                  const QString              type,
                                  const BlockBase::TypeColor typeColor )
    : BlockBase( idHint, systemBlock, type, typeColor ) {
  widget = new ActionDock( mainWindow );
  dock   = new KDDockWidgets::QtWidgets::DockWidget( uniqueName );

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
  BlockBase::setName( name );
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

void
ActionDockBlock::enable( const bool enable ) {
  if( enable ) {
    dock->show();
  } else {
    dock->close();
  }
  BlockBase::enable( enable );
}

std::unique_ptr< BlockBase >
ActionDockBlockFactory::createBlock( BlockBaseId idHint ) {
  auto obj = createBaseBlock< ActionDockBlock >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( firstActionDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    firstActionDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, firstActionDock );
  }

  obj->addOutputPort( QStringLiteral( "Action with State" ), obj.get(), QLatin1StringView( SIGNAL( action( ACTION_SIGNATURE ) ) ) );

  return obj;
}
