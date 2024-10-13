// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PathPlannerModelBlockModel.h"

#include "block/graphical/PathPlannerModel.h"

#include "helpers/BlocksManager.h"

using Type = PathPlannerModel;

QVariant
PathPlannerModelBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  qDebug() << "PathPlannerModelBlockModel::headerData";
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      case 1:
        return QString();
    }
  }

  return QVariant();
}

int
PathPlannerModelBlockModel::rowCount( const QModelIndex& ) const {
  return countBuffer;
}

int
PathPlannerModelBlockModel::columnCount( const QModelIndex& ) const {
  return 2;
}

QVariant
PathPlannerModelBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
    return QVariant();
  }

  for( const auto& blockRef : blocksManager.getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        return block->name();

      case 1:
        return QVariant::fromValue( block );
    }
  }

  return QVariant();
}

bool
PathPlannerModelBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  for( const auto& blockRef : blocksManager.getBlocksWithClass< Type >() | std::ranges::views::drop( index.row() ) ) {
    auto* block = static_cast< Type* >( blockRef.second.get() );

    switch( index.column() ) {
      case 0:
        block->setName( qvariant_cast< QString >( value ) );
        Q_EMIT dataChanged( index, index, QVector< int >() << role );
        return true;
    }
  }

  return false;
}

Qt::ItemFlags
PathPlannerModelBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void
PathPlannerModelBlockModel::resetModel() {
  beginResetModel();

  countBuffer = 0;

  for( const auto& block : blocksManager.getBlocksWithClass< Type >() ) {
    ++countBuffer;
  }

  endResetModel();
}
