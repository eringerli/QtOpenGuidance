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

#include "BlockBase.h"

#include <QComboBox>
#include <QTreeWidget>

void BlockFactory::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
}

void BlockFactory::addToTreeWidget( QTreeWidget* treeWidget ) {

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

QNEBlock* BlockFactory::createBaseBlock( QGraphicsScene* scene, BlockBase* obj, int id, bool systemBlock ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = 0;
    qDebug() << "BlockFactory::createBaseBlock: ID conflict";
  }

  auto* b = new QNEBlock( obj, id, systemBlock );

  scene->addItem( b );

  b->addPort( getPrettyNameOfFactory(), QLatin1String(), false, QNEPort::NamePort );
  b->addPort( getNameOfFactory(),  QLatin1String(), false, QNEPort::TypePort );

  QObject::connect( b, &QNEBlock::emitConfigSignals, obj, &BlockBase::emitConfigSignals );

  return b;
}

bool BlockFactory::isIdUnique( QGraphicsScene* scene, int id ) {
  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( block->id == id ) {
        return false;
      }
    }
  }

  return true;
}

#include <QTreeWidget>
