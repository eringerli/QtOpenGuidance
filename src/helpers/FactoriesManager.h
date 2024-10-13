// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QObject>
#include <vector>

#include "block/BlockBase.h"

using FactoryPointer  = std::unique_ptr< BlockFactory >;
using FactoryCategory = std::vector< FactoryPointer >;

// https://en.wikipedia.org/wiki/Singleton_pattern
class FactoriesManager : public QObject {
  Q_OBJECT

public:
  template< class FactoryClass, class... Args >
  BlockFactory* addFactory( Args... args ) {
    auto factory = std::make_unique< FactoryClass >( args... );

    FactoryCategory* foundCategory = nullptr;
    for( auto& category : _categories ) {
      if( !category.empty() ) {
        if( category.front()->getCategoryOfFactory() == factory->getCategoryOfFactory() ) {
          foundCategory = &category;
        }
      }
    }

    if( foundCategory == nullptr ) {
      _categories.emplace_back();
      foundCategory = &_categories.back();
    }

    auto* factoryPtr = factory.get();

    foundCategory->push_back( std::move( factory ) );
    Q_EMIT factoriesChanged();

    return factoryPtr;
  }

  BlockFactory* getFactoryByName( const QString name ) const;

  const std::vector< FactoryCategory >& categories() const { return _categories; }

signals:
  void factoriesChanged();

private:
  std::vector< FactoryCategory > _categories;
};
