// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PlotBlockModel.h"

#include "block/dock/plot/PlotDockBlockBase.h"

#include "helpers/BlocksManager.h"

using Type = PlotDockBlockBase;

PlotBlockModel::PlotBlockModel( BlocksManager* blocksManager ) : blocksManager( blocksManager ) {}

QVariant
PlotBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      case 1:
        return QStringLiteral( "X Axis Visible" );

      case 2:
        return QStringLiteral( "Y Axis Visible" );

      case 3:
        return QStringLiteral( "Y Axis Description" );

      case 4:
        return QStringLiteral( "Autoscroll Enabled" );

      case 5:
        return QStringLiteral( "Window" );
    }
  }

  return QVariant();
}

Qt::ItemFlags
PlotBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 1 || index.column() == 2 || index.column() == 4 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

int
PlotBlockModel::rowCount( const QModelIndex& ) const {
  return countBuffer;
}

int
PlotBlockModel::columnCount( const QModelIndex& ) const {
  return 6;
}

QVariant
PlotBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::CheckStateRole ) ) {
    return QVariant();
  }

  for( const auto& blockRef : blocksManager->getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->name();
        }
      } break;

      case 1: {
        if( role == Qt::CheckStateRole ) {
          return block->getXAxisVisible() ? Qt::Checked : Qt::Unchecked;
        }
      } break;

      case 2: {
        if( role == Qt::CheckStateRole ) {
          return block->getYAxisVisible() ? Qt::Checked : Qt::Unchecked;
        }
      } break;

      case 3: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->getYAxisDescription();
        }
      } break;

      case 4: {
        if( role == Qt::CheckStateRole ) {
          return block->getAutoscrollEnabled() ? Qt::Checked : Qt::Unchecked;
        }
      } break;

      case 5: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->getWindow();
        }
      } break;
    }
  }

  return QVariant();
}

bool
PlotBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  for( const auto& blockRef : blocksManager->getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        block->setName( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 1:
        block->setXAxisVisible( value.toBool() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 2:
        block->setYAxisVisible( value.toBool() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 3:
        block->setYAxisDescription( value.toString() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 4:
        block->setAutoscrollEnabled( value.toBool() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 5:
        block->setWindow( value.toString().toDouble() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;
    }
  }

  return false;
}

void
PlotBlockModel::resetModel() {
  beginResetModel();

  countBuffer = 0;

  for( const auto& block : blocksManager->getBlocksWithClass< Type >() ) {
    ++countBuffer;
  }
  endResetModel();
}
