// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FactoriesManager.h"
#include <memory>

BlockFactory*
FactoriesManager::getFactoryByName( const QString name ) const {
  for( auto& category : _categories ) {
    for( const auto& factory : category ) {
      if( factory->getNameOfFactory() == name ) {
        return factory.get();
      }
    }
  }

  return nullptr;
}
