// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SliderDockBlockModel.h"

#include "block/dock/input/SliderDockBlock.h"
#include "gui/dock/SliderDock.h"

#include "helpers/BlocksManager.h"

using Type = SliderDockBlock;

SliderDockBlockModel::SliderDockBlockModel( BlocksManager* blocksManager ) : blocksManager( blocksManager ) {}

int
SliderDockBlockModel::columnCount( const QModelIndex& ) const {
  return 9;
}

QVariant
SliderDockBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );
        break;

      case 1:
        return QStringLiteral( "Value" );
        break;

      case 2:
        return QStringLiteral( "Decimals" );
        break;

      case 3:
        return QStringLiteral( "Maximum" );
        break;

      case 4:
        return QStringLiteral( "Minimum" );
        break;

      case 5:
        return QStringLiteral( "DefaultValue" );
        break;

      case 6:
        return QStringLiteral( "Unit" );
        break;

      case 7:
        return QStringLiteral( "Reset on Start?" );
        break;

      case 8:
        return QStringLiteral( "Inverted?" );
        break;
    }
  }

  return QVariant();
}

int
SliderDockBlockModel::rowCount( const QModelIndex& ) const {
  return countBuffer;
}

QVariant
SliderDockBlockModel::data( const QModelIndex& index, int role ) const {
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
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->widget->getValue();
        }
      } break;

      case 2: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->widget->getDecimals();
        }
      } break;

      case 3: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->widget->getMaximum();
        }
      } break;

      case 4: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->widget->getMinimum();
        }
      } break;

      case 5: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->widget->getDefaultValue();
        }
      } break;

      case 6: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->widget->getUnit();
        }
      } break;

      case 7: {
        if( role == Qt::CheckStateRole ) {
          return block->widget->resetOnStart ? Qt::Checked : Qt::Unchecked;
        }
      }

      case 8: {
        if( role == Qt::CheckStateRole ) {
          return block->widget->getSliderInverted();
        }
      }
    }
  }

  return QVariant();
}

bool
SliderDockBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  for( const auto& blockRef : blocksManager->getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        block->setName( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 1:
        block->widget->setValue( value.toString().toDouble() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 2:
        block->widget->setDecimals( value.toString().toInt() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 3:
        block->widget->setMaximum( value.toString().toDouble() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 4:
        block->widget->setMinimum( value.toString().toDouble() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 5:
        block->widget->setDefaultValue( value.toString().toDouble() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 6:
        block->widget->setUnit( value.toString() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 7:
        block->widget->resetOnStart = value.toBool();
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 8:
        block->widget->setSliderInverted( value.toBool() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;
    }
  }

  return false;
}

Qt::ItemFlags
SliderDockBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 7 || index.column() == 8 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void
SliderDockBlockModel::resetModel() {
  beginResetModel();

  countBuffer = 0;

  for( const auto& block : blocksManager->getBlocksWithClass< Type >() ) {
    ++countBuffer;
  }

  endResetModel();
}
