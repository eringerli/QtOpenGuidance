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
  explicit BlocksManager( QObject* parent = nullptr ) : QObject{ parent } {}

  BlockBaseId createBlockFromFactory( const QString factoryName, BlockBaseId idHint = 0 );
  BlockBaseId moveObjectToManager( std::unique_ptr< BlockBase >&& block );

  void registerCallbackForType( const QString& type, std::function< void() > callback ) {
    _typeCallbacks.push_back( std::make_pair( type, callback ) );
  }

  BlockBase* getBlock( const BlockBaseId id ) const {
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
    return _blocks | std::ranges::views::filter( []( const auto& block ) {
             // qDebug() << "getBlocksWithClass" << block.first
             //          << bool( qobject_cast< QObjectDerivedClass* >( block.second.get() ) != nullptr );
             return qobject_cast< QObjectDerivedClass* >( block.second.get() ) != nullptr;
           } );
  }

  void loadConfigFromFile( QFile& file );
  void
  saveConfigToFile( const std::vector< BlockBaseId >& blocks, const std::vector< BlockConnectionDefinition >& connection, QFile& file );

  const std::map< BlockBaseId, std::unique_ptr< BlockBase > >& blocks() const { return _blocks; }

  void disableConnectionOfBlock( const BlockBaseId blockId );
  void enableConnectionOfBlock( const BlockBaseId blockId );

public slots:
  void blockConnectedToDestroyed( QObject* obj );

  void deleteBlock( BlockBaseId blockId, const bool emitObjectsChanged = true );
  void deleteBlocks( std::vector< BlockBaseId >& blocks );

  void deleteConnection( const BlockConnectionDefinition& connection, const bool emitObjectsChanged = true );
  void deleteConnections( const std::vector< BlockConnectionDefinition >& connections );

  void createConnection( const BlockConnectionDefinition& connection, const bool emitObjectsChanged = true );

signals:
  void objectsChanged();

private:
  std::map< BlockBaseId, std::unique_ptr< BlockBase > >               _blocks;
  std::vector< std::pair< const QString&, std::function< void() > > > _typeCallbacks;

  BlockBaseId       _nextSystemId = 0;
  BlockBaseId       _nextUserId   = 1000;
  // FactoriesManager& factoriesManager;

private:
  BlockBaseId getNextSystemId() {
    while( _blocks.contains( ++_nextSystemId ) ) {
    }

    return _nextSystemId;
  }

  BlockBaseId getNextUserId() {
    while( _blocks.contains( ++_nextUserId ) ) {
    }

    return _nextUserId;
  }

  BlockBaseId setNewId( BlockBase& block );

  void callTypeCallbacks( const QString& type );
};

extern BlocksManager blocksManager;
