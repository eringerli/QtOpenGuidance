// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "StringBlockModel.h"

#include "block/literal/StringBlock.h"

#include "helpers/BlocksManager.h"

using Type = StringBlock;

QVariant
StringBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );
        break;

      case 1:
        return QStringLiteral( "String" );
        break;
    }
  }

  return QVariant();
}

int
StringBlockModel::rowCount( const QModelIndex& ) const {
  return countBuffer;
}

int
StringBlockModel::columnCount( const QModelIndex& ) const {
  return 2;
}

QVariant
StringBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
    return QVariant();
  }

  for( const auto& blockRef : blocksManager.getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        return block->name();

      case 1:
        return block->string;
    }
  }

  return QVariant();
}

bool
StringBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  for( const auto& blockRef : blocksManager.getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        block->setName( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 1:
        block->string = value.toString();
        block->emitConfigSignals();
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;
    }
  }

  return false;
}

Qt::ItemFlags
StringBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void
StringBlockModel::resetModel() {
  beginResetModel();

  countBuffer = 0;

  for( const auto& block : blocksManager.getBlocksWithClass< Type >() ) {
    ++countBuffer;
  }

  endResetModel();
}
