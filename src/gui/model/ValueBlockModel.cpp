// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueBlockModel.h"

#include "block/dock/display/ValueDockBlockBase.h"

#include "helpers/BlocksManager.h"

using Type = ValueDockBlockBase;

ValueBlockModel::ValueBlockModel( BlocksManager* blocksManager ) : blocksManager( blocksManager ) {}

QVariant
ValueBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      case 1:
        return QStringLiteral( "Unit Visible" );

      case 2:
        return QStringLiteral( "Unit" );

      case 3:
        return QStringLiteral( "Precision" );

      case 4:
        return QStringLiteral( "Scale" );

      case 5:
        return QStringLiteral( "Field Width" );

      case 6:
        return QStringLiteral( "Font Family" );

      case 7:
        return QStringLiteral( "Font Size" );

      case 8:
        return QStringLiteral( "Bold Font" );
    }
  }

  return QVariant();
}

Qt::ItemFlags
ValueBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 1 || index.column() == 8 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

int
ValueBlockModel::rowCount( const QModelIndex& ) const {
  return countBuffer;
}

int
ValueBlockModel::columnCount( const QModelIndex& ) const {
  return 9;
}

QVariant
ValueBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
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
          return block->unitVisible() ? Qt::Checked : Qt::Unchecked;
        }
      } break;

      case 2: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->getUnit();
        }
      } break;

      case 3: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->getPrecision();
        }
      } break;

      case 4: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->getScale();
        }
      } break;

      case 5: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->getFieldWidth();
        }
      } break;

      case 6: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->getFont();
        }
      } break;

      case 7: {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
          return block->getFont().pointSize();
        }
      } break;

      case 8: {
        if( role == Qt::CheckStateRole ) {
          return block->getFont().bold() ? Qt::Checked : Qt::Unchecked;
        }
      } break;
    }
  }

  return QVariant();
}

bool
ValueBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  for( const auto& blockRef : blocksManager->getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        block->setName( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 1:
        block->setUnitVisible( value.toBool() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 2:
        block->setUnit( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 3:
        block->setPrecision( value.toString().toInt() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 4:
        block->setScale( value.toString().toFloat() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 5:
        block->setFieldWidth( value.toString().toInt() );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 6: {
        auto  font    = value.value< QFont >();
        QFont oldFont = block->getFont();
        font.setBold( oldFont.bold() );
        font.setPointSize( oldFont.pointSize() );
        block->setFont( font );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;
      }

      case 7: {
        QFont font = block->getFont();
        font.setPointSize( value.toInt() );
        block->setFont( font );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;
      }

      case 8: {
        QFont font = block->getFont();
        font.setBold( value.toBool() );
        block->setFont( font );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;
      }
    }
  }

  return false;
}

void
ValueBlockModel::resetModel() {
  beginResetModel();

  countBuffer = 0;

  for( const auto& block : blocksManager->getBlocksWithClass< Type >() ) {
    ++countBuffer;
  }
  endResetModel();
}
