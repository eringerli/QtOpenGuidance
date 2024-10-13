// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TransmissionBlockModel.h"

#include "block/converter/ValueTransmissionBase.h"
#include "block/converter/ValueTransmissionBase64Data.h"
#include "block/converter/ValueTransmissionImuData.h"
#include "block/converter/ValueTransmissionNumber.h"
#include "block/converter/ValueTransmissionQuaternion.h"
#include "block/converter/ValueTransmissionState.h"

#include "helpers/BlocksManager.h"

using Type = ValueTransmissionBase;

TransmissionBlockModel::TransmissionBlockModel( BlocksManager* blocksManager ) : blocksManager( blocksManager ) {}

QVariant
TransmissionBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      case 1:
        return QStringLiteral( "Type" );

      case 2:
        return QStringLiteral( "Channel ID" );

      case 3:
        return QStringLiteral( "Timeout ms" );

      case 4:
        return QStringLiteral( "Repetition ms" );
    }
  }

  return QVariant();
}

Qt::ItemFlags
TransmissionBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 1 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

int
TransmissionBlockModel::rowCount( const QModelIndex& ) const {
  return countBuffer;
}

int
TransmissionBlockModel::columnCount( const QModelIndex& ) const {
  return 5;
}

QVariant
TransmissionBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
    return QVariant();
  }

  for( const auto& blockRef : blocksManager->getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        return block->name();

      case 1: {
        if( qobject_cast< ValueTransmissionBase64Data* >( block ) != nullptr ) {
          return QStringLiteral( "Data" );
        }

        if( qobject_cast< ValueTransmissionNumber* >( block ) != nullptr ) {
          return QStringLiteral( "Number" );
        }

        if( qobject_cast< ValueTransmissionQuaternion* >( block ) != nullptr ) {
          return QStringLiteral( "Quaternion" );
        }

        if( qobject_cast< ValueTransmissionState* >( block ) != nullptr ) {
          return QStringLiteral( "State" );
        }

        if( qobject_cast< ValueTransmissionImuData* >( block ) != nullptr ) {
          return QStringLiteral( "IMU Data" );
        }

        return QStringLiteral( "Unknown" );
      }

      case 2:
        return block->cid;

      case 3:
        return block->timeoutTimeMs;

      case 4:
        return block->repeatTimeMs;
    }
  }

  return QVariant();
}

bool
TransmissionBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  for( const auto& blockRef : blocksManager->getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        block->setName( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 2:
        block->cid = value.toString().toInt();
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 3:
        block->timeoutTimeMs = value.toString().toInt();
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 4:
        block->repeatTimeMs = value.toString().toInt();
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      default:
        break;
    }
  }

  return false;
}

void
TransmissionBlockModel::resetModel() {
  beginResetModel();

  countBuffer = 0;

  for( const auto& block : blocksManager->getBlocksWithClass< Type >() ) {
    ++countBuffer;
  }

  endResetModel();
}
