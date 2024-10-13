// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"
#include "helpers/FactoriesManager.h"

#include <QObject>

#include <QFile>
#include <functional>
#include <memory>
#include <ranges>
#include <utility>

class BlocksManager : public QObject {
  Q_OBJECT
public:
  explicit BlocksManager( FactoriesManager& factoriesManager, QObject* parent = nullptr );

  int createBlockFromFactory( const QString factoryName, int idHint = 0 );
  int moveObjectToManager( std::unique_ptr< BlockBase >&& block );

  void registerCallbackForType( const QString& type, std::function< void() > callback ) {
    _typeCallbacks.push_back( std::make_pair( type, callback ) );
  }

  BlockBase* getBlock( const int id ) const {
    if( _blocks.contains( id ) ) {
      return _blocks.at( id ).get();
    } else {
      return nullptr;
    }
  }

  auto getBlocksWithType( const QString& type ) const {
    return _blocks | std::ranges::views::filter( [&type]( const auto& block ) { return block.second->type == type; } );
  }

  template< class QObjectDerivedClass >
  auto getBlocksWithClass() const {
    return _blocks | std::ranges::views::filter(
                       []( const auto& block ) { return qobject_cast< QObjectDerivedClass* >( block.second.get() ) != nullptr; } );
  }

  void loadConfigFromFile( QFile& file );
  void saveConfigToFile( const std::vector< int >& blocks, const std::vector< BlockConnectionDefinition >& connection, QFile& file );

  const std::map< int, std::unique_ptr< BlockBase > >& blocks() const { return _blocks; }

public slots:
  void blockConnectedToDestroyed( QObject* obj );

  void deleteBlock( int blockId, const bool emitObjectsChanged = true );
  void deleteBlocks( std::vector< int >& blocks );

  void deleteConnection( const BlockConnectionDefinition& connection, const bool emitObjectsChanged = true );
  void deleteConnections( const std::vector< BlockConnectionDefinition >& connections );

  void createConnection( const BlockConnectionDefinition& connection, const bool emitObjectsChanged = true );

signals:
  void objectsChanged();

private:
  std::map< int, std::unique_ptr< BlockBase > >                       _blocks;
  std::vector< std::pair< const QString&, std::function< void() > > > _typeCallbacks;

  int               _nextSystemId = 0;
  int               _nextUserId   = 1000;
  FactoriesManager& factoriesManager;

private:
  int getNextSystemId() {
    while( _blocks.contains( ++_nextSystemId ) ) {
    }

    return _nextSystemId;
  }

  int getNextUserId() {
    while( _blocks.contains( ++_nextUserId ) ) {
    }

    return _nextUserId;
  }

  int setNewId( BlockBase& block );

  void callTypeCallbacks( const QString& type );
};
