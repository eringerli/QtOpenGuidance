// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BlockBase.h"
#include "helpers/BlocksManager.h"

#include <algorithm>
#include <bits/ranges_util.h>
#include <tuple>

void
BlockBase::enable( const bool enable ) {
  if( enable ) {
    blocksManager.enableConnectionOfBlock( id() );
  } else {
    blocksManager.disableConnectionOfBlock( id() );
  }
}

const BlockConnection*
BlockBase::getConnection( const BlockConnectionDefinition connectionDefinition ) {
  for( const auto& connection : _connections ) {
    if( connection == connectionDefinition ) {
      return &connection;
    }
  }

  return nullptr;
}

void
BlockBase::setId( BlockBaseId newId ) {
  _id = newId;
  for( auto& port : _ports ) {
    port.idOfBlock = newId;
  }
  for( auto& connection : _connections ) {
    connection.portFrom->idOfBlock = newId;
  }
}

void
BlockBase::setName( const QString& name ) {
  _name = name;
  Q_EMIT nameChanged( name );
}

void
BlockBase::toJSONBase( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "id" )]        = _id;
  valuesObject[QStringLiteral( "type" )]      = type;
  valuesObject[QStringLiteral( "name" )]      = _name;
  valuesObject[QStringLiteral( "positionX" )] = positionX;
  valuesObject[QStringLiteral( "positionY" )] = positionY;

  {
    auto json = QJsonObject();
    toJSON( json );
    if( !json.isEmpty() ) {
      valuesObject[QStringLiteral( "value0" )] = json;
    }
  }

  {
    int counter = 1;
    for( auto& obj : _additionalObjects ) {
      auto* block = qobject_cast< BlockBase* >( obj );
      if( block != nullptr ) {
        auto json = QJsonObject();
        block->toJSON( json );

        if( !json.isEmpty() ) {
          auto key = QStringLiteral( "value" ) + QString::number( counter );

          valuesObject[key] = json;
        }
      }
      counter++;
    }
  }
}

void
BlockBase::fromJSONBase( const QJsonObject& valuesObject ) {
  positionX = valuesObject[QStringLiteral( "positionX" )].toDouble( 0 );
  positionY = valuesObject[QStringLiteral( "positionY" )].toDouble( 0 );

  if( valuesObject[QStringLiteral( "value0" )].isObject() ) {
    fromJSON( valuesObject[QStringLiteral( "value0" )].toObject() );
  }

  {
    int counter = 1;
    for( auto& obj : _additionalObjects ) {
      auto key = QStringLiteral( "value" ) + QString::number( counter );
      if( valuesObject[key].isObject() ) {
        auto* block = qobject_cast< BlockBase* >( obj );
        if( block != nullptr ) {
          block->fromJSON( valuesObject[key].toObject() );
        }
      }
      counter++;
    }
  }

  setName( valuesObject[QStringLiteral( "name" )].toString( type ) );
}

void
BlockBase::addAdditionalObject( QObject* object ) {
  _additionalObjects.push_back( object );
}

BlockPort&
BlockBase::addInputPort( const QString& name, QObject* obj, QLatin1StringView signature, BlockPort::Flags flags ) {
  return addPort( name, obj, signature, flags );
}

BlockPort&
BlockBase::addOutputPort( const QString& name, QObject* obj, QLatin1StringView signature, BlockPort::Flags flags ) {
  return addPort( name, obj, signature, flags | BlockPort::Flag::Output );
}

BlockPort&
BlockBase::addPort( const QString& name, QObject* obj, QLatin1StringView signatureStringView, BlockPort::Flags flags ) {
  auto signature = QString( signatureStringView );
  signature.remove( 0, 1 );

  if( obj != nullptr ) {
    QByteArray  normalizedSignature = QMetaObject::normalizedSignature( signature.toLatin1() );
    int         methodIndex         = obj->metaObject()->indexOfMethod( normalizedSignature );
    QMetaMethod method              = obj->metaObject()->method( methodIndex );

    if( method.isValid() ) {
      return _ports.emplace_back( _id, _portPosition++, name, obj, method, flags );
    }
  }

  return _ports.emplace_back( _id, _portPosition++, name, flags );
}

size_t
BlockBase::connectPortToPort( const QString& portFromName, const BlockBase* blockTo, const QString& portToName ) {
  size_t      count = 0;
  BlockBaseId from = 0, to = 0;
  for( auto& portFrom : getOutputPorts( portFromName ) ) {
    from++;
    for( auto& portTo : blockTo->getInputPorts( portToName ) ) {
      to++;
      auto connection = QObject::connect(
        portFrom.object, portFrom.method, portTo.object, portTo.method, Qt::ConnectionType( Qt::AutoConnection | Qt::UniqueConnection ) );
      if( connection ) {
        // TODO
        _connections.emplace_back( ( BlockPort* )&portFrom, ( BlockPort* )&portTo, std::move( connection ) );
        ++count;
      }
    }
  }

  return count;
}

void
BlockBase::disconnectPortToPort( const QString& portFromName, const BlockBase* blockTo, const QString& portToName ) {
  if( blockTo != nullptr ) {
    for( auto it = _connections.begin(); it != _connections.end(); ) {
      if( it->portTo->idOfBlock == blockTo->id() && it->portFrom->name == portFromName && it->portTo->name == portToName ) {
        it->disconnect();
        it = _connections.erase( it );
      } else {
        ++it;
      }
    }
  }
}

const BlockConnectionDefinition
BlockConnection::getDefinition() const {
  if( portFrom != nullptr && portTo != nullptr ) {
    return BlockConnectionDefinition( portFrom->idOfBlock, portFrom->name, portTo->idOfBlock, portTo->name );
  } else {
    qDebug() << "BlockConnectionDefinition( 0, \"\", 0, \"\" )" << portFrom << portTo;
    return BlockConnectionDefinition( 0, "", 0, "" );
  }
}

void
BlockConnection::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "idFrom" )]   = portFrom->idOfBlock;
  valuesObject[QStringLiteral( "portFrom" )] = portFrom->name;
  valuesObject[QStringLiteral( "idTo" )]     = portTo->idOfBlock;
  valuesObject[QStringLiteral( "portTo" )]   = portTo->name;
}
