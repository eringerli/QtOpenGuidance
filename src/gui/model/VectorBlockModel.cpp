// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "VectorBlockModel.h"

#include "block/literal/VectorBlock.h"

#include "helpers/BlocksManager.h"

using Type = VectorBlock;

QVariant
VectorBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      case 1:
        return QStringLiteral( "X" );

      case 2:
        return QStringLiteral( "Y" );

      case 3:
        return QStringLiteral( "Z" );

      default:
        return QString();
    }
  }

  return QVariant();
}

int
VectorBlockModel::rowCount( const QModelIndex& ) const {
  return countBuffer;
}

int
VectorBlockModel::columnCount( const QModelIndex& ) const {
  return 4;
}

QVariant
VectorBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
    return QVariant();
  }

  for( const auto& blockRef : blocksManager.getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        return block->name();

      case 1:
        return block->vector.x();

      case 2:
        return block->vector.y();

      case 3:
        return block->vector.z();
    }
  }

  return QVariant();
}

bool
VectorBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  for( const auto& blockRef : blocksManager.getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        block->setName( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 1:
        block->vector.x() = value.toString().toDouble();
        block->emitConfigSignals();
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 2:
        block->vector.y() = value.toString().toDouble();
        block->emitConfigSignals();
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 3:
        block->vector.z() = value.toString().toDouble();
        block->emitConfigSignals();
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;
    }
  }

  return false;
}

Qt::ItemFlags
VectorBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void
VectorBlockModel::resetModel() {
  beginResetModel();

  countBuffer = 0;

  for( const auto& block : blocksManager.getBlocksWithClass< Type >() ) {
    ++countBuffer;
  }

  endResetModel();
}
