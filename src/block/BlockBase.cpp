// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include <QComboBox>
#include <QGraphicsScene>
#include <QTreeWidget>

void
BlockFactory::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
}

void
BlockFactory::addToTreeWidget( QTreeWidget* treeWidget ) {
  auto results = treeWidget->findItems( getCategoryOfFactory(), Qt::MatchExactly );

  QTreeWidgetItem* parentItem = nullptr;

  for( auto* item : qAsConst( results ) ) {
    // top level entry?
    if( item->parent() == nullptr ) {
      parentItem = item;
    }
  }

  if( parentItem == nullptr ) {
    parentItem = new QTreeWidgetItem( treeWidget );
    parentItem->setText( 0, getCategoryOfFactory() );
  }

  auto* newItem = new QTreeWidgetItem( parentItem );
  newItem->setText( 0, getPrettyNameOfFactory() );
  newItem->setText( 1, getNameOfFactory() );
  newItem->setData( 0, Qt::UserRole, QVariant::fromValue( this ) );
}

QNEBlock*
BlockFactory::createBaseBlock( QGraphicsScene* scene, BlockBase* obj, int id, bool systemBlock ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = 0;
  }

  auto* b = new QNEBlock( obj, id, systemBlock );

  scene->addItem( b );

  b->addPort( getPrettyNameOfFactory(), QLatin1String(), false, QNEPort::NamePort );
  b->addPort( getNameOfFactory(), QLatin1String(), false, QNEPort::TypePort );

  QObject::connect( b, &QNEBlock::emitConfigSignals, obj, &BlockBase::emitConfigSignals );

  return b;
}

bool
BlockFactory::isIdUnique( QGraphicsScene* scene, int id ) {
  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( block->id == id ) {
        return false;
      }
    }
  }

  return true;
}

const QColor BlockFactory::modelColor       = QColor( QStringLiteral( "moccasin" ) );
const QColor BlockFactory::dockColor        = QColor( QStringLiteral( "DarkSalmon" ) );
const QColor BlockFactory::inputDockColor   = QColor( QStringLiteral( "lightsalmon" ) );
const QColor BlockFactory::parserColor      = QColor( QStringLiteral( "mediumaquamarine" ) );
const QColor BlockFactory::valueColor       = QColor( QStringLiteral( "gold" ) );
const QColor BlockFactory::inputOutputColor = QColor( QStringLiteral( "cornflowerblue" ) );
const QColor BlockFactory::converterColor   = QColor( QStringLiteral( "lightblue" ) );
const QColor BlockFactory::arithmeticColor  = QColor( QStringLiteral( "DarkKhaki" ) );
