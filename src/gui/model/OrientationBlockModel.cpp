// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OrientationBlockModel.h"

#include "block/literal/OrientationBlock.h"

#include "helpers/BlocksManager.h"

#include "helpers/anglesHelper.h"
#include "helpers/eigenHelper.h"

using Type = OrientationBlock;

OrientationBlockModel::OrientationBlockModel( BlocksManager* blocksManager ) : blocksManager( blocksManager ) {}

QVariant
OrientationBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      case 1:
        return QStringLiteral( "Yaw" );

      case 2:
        return QStringLiteral( "Pitch" );

      case 3:
        return QStringLiteral( "Roll" );

      case 4:
        return QStringLiteral( "x" );

      case 5:
        return QStringLiteral( "y" );

      case 6:
        return QStringLiteral( "z" );

      case 7:
        return QStringLiteral( "w" );
    }
  }

  return QVariant();
}

int
OrientationBlockModel::rowCount( const QModelIndex& ) const {
  return countBuffer;
}

int
OrientationBlockModel::columnCount( const QModelIndex& ) const {
  return 8;
}

QVariant
OrientationBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
    return QVariant();
  }

  for( const auto& blockRef : blocksManager->getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        return block->name();

      case 1:
        return radiansToDegrees( getYaw( quaternionToTaitBryan( block->orientation ) ) );

      case 2:
        return radiansToDegrees( getPitch( quaternionToTaitBryan( block->orientation ) ) );

      case 3:
        return radiansToDegrees( getRoll( quaternionToTaitBryan( block->orientation ) ) );

      case 4:
        return block->orientation.x();

      case 5:
        return block->orientation.y();

      case 6:
        return block->orientation.z();

      case 7:
        return block->orientation.w();
    }
  }

  return QVariant();
}

bool
OrientationBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  for( const auto& blockRef : blocksManager->getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    auto taitBryan = quaternionToTaitBryan( block->orientation );

    switch( index.column() ) {
      case 0:
        block->setName( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;

      case 1:
        block->orientation =
          taitBryanToQuaternion( degreesToRadians( value.toString().toDouble() ), getPitch( taitBryan ), getRoll( taitBryan ) );
        block->emitConfigSignals();
        Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
        return true;

      case 2:
        block->orientation =
          taitBryanToQuaternion( getYaw( taitBryan ), degreesToRadians( value.toString().toDouble() ), getRoll( taitBryan ) );
        block->emitConfigSignals();
        Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
        return true;

      case 3:
        block->orientation =
          taitBryanToQuaternion( getYaw( taitBryan ), getPitch( taitBryan ), degreesToRadians( value.toString().toDouble() ) );
        block->emitConfigSignals();
        Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
        return true;

      case 4:
        block->orientation.x() = value.toString().toDouble();
        block->orientation.normalize();
        block->emitConfigSignals();
        Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
        return true;

      case 5:
        block->orientation.y() = value.toString().toDouble();
        block->orientation.normalize();
        block->emitConfigSignals();
        Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
        return true;

      case 6:
        block->orientation.z() = value.toString().toDouble();
        block->orientation.normalize();
        block->emitConfigSignals();
        Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
        return true;

      case 7:
        block->orientation.w() = value.toString().toDouble();
        block->orientation.normalize();
        block->emitConfigSignals();
        Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
        return true;
    }
  }

  return false;
}

Qt::ItemFlags
OrientationBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void
OrientationBlockModel::resetModel() {
  beginResetModel();

  countBuffer = 0;

  for( const auto& block : blocksManager->getBlocksWithClass< Type >() ) {
    ++countBuffer;
  }

  endResetModel();
}
