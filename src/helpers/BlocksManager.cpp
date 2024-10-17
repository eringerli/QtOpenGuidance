// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BlocksManager.h"
#include "block/BlockBase.h"
#include "qjsonobject.h"
#include "qtmetamacros.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>
#include <ranges>
#include <utility>

BlockBaseId
BlocksManager::createBlockFromFactory( const QString factoryName, BlockBaseId idHint ) {
  auto* factory = factoriesManager.getFactoryByName( factoryName );

  if( factory ) {
    return moveObjectToManager( factory->createBlock( idHint ) );
  } else {
    return 0;
  }
}

BlockBaseId
BlocksManager::moveObjectToManager( std::unique_ptr< BlockBase >&& block ) {
  const bool idExistentOrZero = ( block->id() == 0 ) || ( _blocks.contains( block->id() ) );

  //  qDebug() << "BlocksManager::moveObjectToManager" << ( block->id() == 0 ) << ( _blocks.contains( block->id() ) );

  BlockBaseId newId = block->id();
  if( idExistentOrZero ) {
    BlockBaseId oldId = newId;
    newId             = setNewId( *block );
    //    qDebug() << "oldId << newId" << block->name() << oldId << newId;
  }

  BlockBase* newPtr = block.get();

  _blocks[newId] = std::move( block );

  callTypeCallbacks( block->type );

  Q_EMIT objectsChanged();

  return newId;
}

void
BlocksManager::loadConfigFromFile( QFile& file ) {
  blockSignals( true );

  QByteArray saveData = file.readAll();

  QJsonDocument loadDoc( QJsonDocument::fromJson( saveData ) );
  QJsonObject   json = loadDoc.object();

  // as the new object (can) get new id, here is a st::map to hold the conversions
  // first int: id in file, second int: id in the block list
  std::map< BlockBaseId, BlockBaseId > mapIdFileToBlockVector;

  if( json.contains( QStringLiteral( "blocks" ) ) && json[QStringLiteral( "blocks" )].isArray() ) {
    QJsonArray blocksArray = json[QStringLiteral( "blocks" )].toArray();

    for( const auto& blockIndex : std::as_const( blocksArray ) ) {
      QJsonObject blockObject = blockIndex.toObject();
      BlockBaseId id          = blockObject[QStringLiteral( "id" )].toInt( 0 );

      // search the block and set the values
      if( id != 0 ) {
        if( id < BlockBaseId( BlockBase::IdRange::UserIdStart ) ) {
          // system id -> don't create new blocks
          BlockBase* block = nullptr;
          //          qDebug() << "SystemBlock" << blockObject[QStringLiteral( "type" )].toString();
          if( !blockObject[QStringLiteral( "type" )].isUndefined() ) {
            for( const auto& blockOfType : getBlocksWithType( blockObject[QStringLiteral( "type" )].toString() ) ) {
              block = blockOfType.second.get();
            }
          } else {
            block = getBlock( id );
          }

          if( block != nullptr ) {
            mapIdFileToBlockVector[id] = block->id();

            block->fromJSONBase( blockObject );

            //            qDebug() << "SystemBlock (" << block->type << ")" << id << "->" << block->id() << block->name();
          }

        } else {
          // id is not a system-id -> create new blocks
          auto newId = createBlockFromFactory( blockObject[QStringLiteral( "type" )].toString(), id );
          if( newId != 0 ) {
            mapIdFileToBlockVector[id] = newId;

            _blocks[newId]->fromJSONBase( blockObject );
            //            qDebug() << "Block (" << _blocks[newId]->type << ")" << id << "->" << newId << _blocks[newId]->name();
          }
        }
      }
    }
  }

  if( json.contains( QStringLiteral( "connections" ) ) && json[QStringLiteral( "connections" )].isArray() ) {
    QJsonArray connectionsArray = json[QStringLiteral( "connections" )].toArray();

    for( const auto& connectionsIndex : std::as_const( connectionsArray ) ) {
      QJsonObject connectionsObject = connectionsIndex.toObject();

      if( !connectionsObject[QStringLiteral( "idFrom" )].isUndefined() && !connectionsObject[QStringLiteral( "idTo" )].isUndefined() &&
          !connectionsObject[QStringLiteral( "portFrom" )].isUndefined() && !connectionsObject[QStringLiteral( "portTo" )].isUndefined() ) {
        BlockBaseId idFrom = mapIdFileToBlockVector[connectionsObject[QStringLiteral( "idFrom" )].toInt()];
        BlockBaseId idTo   = mapIdFileToBlockVector[connectionsObject[QStringLiteral( "idTo" )].toInt()];

        QString portFromName = connectionsObject[QStringLiteral( "portFrom" )].toString();
        QString portToName   = connectionsObject[QStringLiteral( "portTo" )].toString();

        if( idFrom != 0 && idTo != 0 && !portFromName.isEmpty() && !portToName.isEmpty() ) {
          auto connection = BlockConnectionDefinition( idFrom, portFromName, idTo, portToName );

          createConnection( connection, false );
        } else {
          qDebug() << "( blockFrom != nullptr ) && ( blockTo != nullptr )s";
        }
      } else {
        qDebug() << "idFrom != 0 && idTo != 0";
      }
    }
  }

  // as new values for the blocks are added above, emit all signals now, when the
  // connections are made
  for( const auto& block : _blocks ) {
    block.second->emitConfigSignals();
  }

  blockSignals( false );

  Q_EMIT objectsChanged();
}

void
BlocksManager::saveConfigToFile( const std::vector< BlockBaseId >&               blocksConst,
                                 const std::vector< BlockConnectionDefinition >& connectionsConst,
                                 QFile&                                          file ) {
  QJsonObject jsonObject;

  auto jsonBlocks      = QJsonArray();
  auto jsonConnections = QJsonArray();

  auto blocks      = blocksConst;
  auto connections = connectionsConst;

  std::sort( blocks.begin(), blocks.end() );
  std::sort( connections.begin(), connections.end() );

  for( const auto blockId : blocks ) {
    auto* block = getBlock( blockId );
    if( block != nullptr ) {
      QJsonObject json;
      block->toJSONBase( json );
      jsonBlocks.append( json );
    }
  }

  for( const auto& connectionDefinition : connections ) {
    auto* block = getBlock( connectionDefinition.idFrom );
    if( block != nullptr ) {
      const auto* connection = block->getConnection( connectionDefinition );
      if( connection != nullptr ) {
        QJsonObject json;
        connection->toJSON( json );
        jsonConnections.append( json );
      }
    }
  }

  jsonObject[QStringLiteral( "blocks" )]      = jsonBlocks;
  jsonObject[QStringLiteral( "connections" )] = jsonConnections;

  QJsonDocument jsonDocument( jsonObject );
  file.write( jsonDocument.toJson() );
}

void
BlocksManager::blockConnectedToDestroyed( const QObject* obj ) {
  const auto* block = qobject_cast< const BlockBase* >( obj );
  if( block != nullptr ) {
    deleteBlock( block->id() );
  }
}

void
BlocksManager::deleteBlock( const BlockBaseId blockId, const bool emitObjectsChanged ) {
  for( const auto& block : _blocks ) {
    for( auto& connection : block.second->connections() ) {
      if( ( connection.portFrom->idOfBlock == blockId ) || ( connection.portTo->idOfBlock == blockId ) ) {
        deleteConnection(
          BlockConnectionDefinition(
            connection.portFrom->idOfBlock, connection.portFrom->name, connection.portTo->idOfBlock, connection.portTo->name ),
          false );
      }
    }
  }

  auto* block = getBlock( blockId );
  if( ( block != nullptr ) && !block->systemBlock ) {
    _blocks.erase( block->id() );
  }

  if( emitObjectsChanged ) {
    Q_EMIT objectsChanged();
  }
}

void
BlocksManager::deleteConnection( const BlockConnectionDefinition& connection, const bool emitObjectsChanged ) {
  auto blockFromPtr = getBlock( connection.idFrom );
  auto blockToPtr   = getBlock( connection.idTo );

  if( blockFromPtr != nullptr && blockToPtr != nullptr ) {
    blockFromPtr->disconnectPortToPort( connection.portFromName, blockToPtr, connection.portToName );
  }

  if( emitObjectsChanged ) {
    Q_EMIT objectsChanged();
  }
}

void
BlocksManager::createConnection( const BlockConnectionDefinition& connection, const bool emitObjectsChanged ) {
  auto blockFromPtr = getBlock( connection.idFrom );
  auto blockToPtr   = getBlock( connection.idTo );

  if( blockFromPtr != nullptr && blockToPtr != nullptr ) {
    auto count = blockFromPtr->connectPortToPort( connection.portFromName, blockToPtr, connection.portToName );

    if( count == 0 ) {
      count = blockToPtr->connectPortToPort( connection.portToName, blockFromPtr, connection.portFromName );
    }

    if( emitObjectsChanged && count ) {
      Q_EMIT objectsChanged();
    }
  }
}

void
BlocksManager::deleteBlocks( const std::vector< BlockBaseId >& blocks ) {
  for( const auto& block : blocks ) {
    deleteBlock( block, false );
  }

  Q_EMIT objectsChanged();
}

void
BlocksManager::deleteConnections( const std::vector< BlockConnectionDefinition >& connections ) {
  for( const auto& connection : connections ) {
    deleteConnection( connection, false );
  }

  Q_EMIT objectsChanged();
}

BlockBaseId
BlocksManager::setNewId( BlockBase& block ) {
  if( block.systemBlock ) {
    block.setId( getNextSystemId() );
  } else {
    block.setId( getNextUserId() );
  }
  return block.id();
}

void
BlocksManager::callTypeCallbacks( const QString& type ) {
  for( const auto& callback :
       _typeCallbacks | std::ranges::views::filter( [&type]( const auto& callback ) { return callback.first == type; } ) ) {
    callback.second();
  }
}

void
BlocksManager::disableConnectionOfBlock( const BlockBaseId blockId ) {
  auto blocks = std::views::values( _blocks );
  for( auto& block : blocks ) {
    for( auto& connection : block->connections() ) {
      if( ( ( connection.portFrom->idOfBlock == blockId ) && ( !connection.portFrom->flags.testFlags( BlockPort::Flag::SquareBullet ) ) ) ||
          ( ( connection.portTo->idOfBlock == blockId ) && ( !connection.portTo->flags.testFlags( BlockPort::Flag::SquareBullet ) ) ) ) {
        connection.disable();
      }
    }
  }
}

void
BlocksManager::enableConnectionOfBlock( const BlockBaseId blockId ) {
  auto blocks = std::views::values( _blocks );
  for( auto& block : blocks ) {
    for( auto& connection : block->connections() ) {
      if( ( ( connection.portFrom->idOfBlock == blockId ) && ( !connection.portFrom->flags.testFlags( BlockPort::Flag::SquareBullet ) ) ) ||
          ( ( connection.portTo->idOfBlock == blockId ) && ( !connection.portTo->flags.testFlags( BlockPort::Flag::SquareBullet ) ) ) ) {
        connection.enable();
      }
    }
  }
}

BlocksManager blocksManager;
