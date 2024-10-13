// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FactoriesModel.h"

#include <algorithm>

#include "QStandardItemModel"
#include "helpers/FactoriesManager.h"

FactoriesModel::FactoriesModel( FactoriesManager* factoriesManager, QObject* parent )
    : QStandardItemModel( parent ), factoriesManager( factoriesManager ) {
  resetModel();

  QObject::connect( factoriesManager, &FactoriesManager::factoriesChanged, this, &FactoriesModel::resetModel );
}

void
FactoriesModel::resetModel() {
  beginResetModel();

  clear();

  setColumnCount( 1 );

  auto* rootItem = invisibleRootItem();

  for( const auto& category : factoriesManager->categories() ) {
    if( !category.empty() ) {
      auto categoryItem = new QStandardItem( category.front()->getCategoryOfFactory() );
      categoryItem->setFlags( Qt::ItemIsEnabled );
      rootItem->appendRow( categoryItem );

      for( const auto& factory : category ) {
        auto factoryItem = new QStandardItem();
        factoryItem->setText( factory->getPrettyNameOfFactory() );
        factoryItem->setData( factory->getNameOfFactory() );
        factoryItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

        categoryItem->appendRow( factoryItem );
      }
    }
  }

  endResetModel();
}
