// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ActionDockBlockModel.h"

#include "block/dock/input/ActionDockBlock.h"
#include "gui/dock/ActionDock.h"

#include "helpers/BlocksManager.h"

using Type = ActionDockBlock;

int
ActionDockBlockModel::columnCount( const QModelIndex& ) const {
  return 4;
}

QVariant
ActionDockBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );
        break;

      case 1:
        return QStringLiteral( "State" );
        break;

      case 2:
        return QStringLiteral( "Checkable" );
        break;

      case 3:
        return QStringLiteral( "Icon" );
        break;
    }
  }

  return QVariant();
}

int
ActionDockBlockModel::rowCount( const QModelIndex& ) const {
  return countBuffer;
}

QVariant
ActionDockBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::CheckStateRole ) ) {
    return QVariant();
  }

  for( const auto& blockRef : blocksManager.getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->name();
        }
      } break;

      case 1: {
        if( role == Qt::CheckStateRole ) {
          return block->widget->state() ? Qt::Checked : Qt::Unchecked;
        }
      } break;
      case 2: {
        if( role == Qt::CheckStateRole ) {
          return block->widget->isCheckable() ? Qt::Checked : Qt::Unchecked;
        }
      } break;

      case 3: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->widget->getTheme();
        }
      } break;
    }
  }

  return QVariant();
}

bool
ActionDockBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  for( const auto& blockRef : blocksManager.getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        block->setName( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 1:
        block->widget->setState( value.toBool() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 2:
        block->widget->setCheckable( value.toBool() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 3:
        block->widget->setTheme( value.toString() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;
    }
  }

  return false;
}

Qt::ItemFlags
ActionDockBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 1 || index.column() == 2 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void
ActionDockBlockModel::resetModel() {
  beginResetModel();

  countBuffer = 0;

  for( const auto& block : blocksManager.getBlocksWithClass< Type >() ) {
    ++countBuffer;
  }
  endResetModel();
}
