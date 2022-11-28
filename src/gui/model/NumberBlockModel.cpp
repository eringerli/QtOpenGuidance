// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "NumberBlockModel.h"

#include "block/literal/NumberObject.h"

#include <QComboBox>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qneblock.h"

NumberBlockModel::NumberBlockModel( QGraphicsScene* scene ) : scene( scene ) {}

QVariant
NumberBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );
        break;

      case 1:
        return QStringLiteral( "Number" );
        break;

      default:
        return QString();
        break;
    }
  }

  return QVariant();
}

bool
NumberBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    // FIXME: Implement me!
    Q_EMIT headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}

int
NumberBlockModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

int
NumberBlockModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 2;
}

QVariant
NumberBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
    return QVariant();
  }

  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( auto* object = qobject_cast< NumberObject* >( block->objects.front() ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              return block->getName();

            case 1:
              return object->number;
          }
        }
      }
    }
  }

  return QVariant();
}

bool
NumberBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( auto* object = qobject_cast< NumberObject* >( block->objects.front() ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast< QString >( value ) );
              Q_EMIT dataChanged( index, index, QVector< int >() << role );
              return true;

            case 1:
              object->number = value.toString().toFloat();
              object->emitConfigSignals();
              Q_EMIT dataChanged( index, index, QVector< int >() << role );
              return true;
          }
        }
      }
    }
  }

  return false;
}

Qt::ItemFlags
NumberBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void
NumberBlockModel::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( QStringLiteral( "Number" ), QVariant::fromValue( this ) );
}

void
NumberBlockModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( qobject_cast< NumberObject* >( block->objects.front() ) != nullptr ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}
