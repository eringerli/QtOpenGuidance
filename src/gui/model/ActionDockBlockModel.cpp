// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ActionDockBlockModel.h"

#include "block/dock/input/ActionDockBlock.h"
#include "gui/dock/ActionDock.h"

#include <QComboBox>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qneblock.h"

ActionDockBlockModel::ActionDockBlockModel( QGraphicsScene* scene ) : scene( scene ) {}

int
ActionDockBlockModel::columnCount( const QModelIndex& /*parent*/ ) const {
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

      default:
        return QString();
        break;
    }
  }

  return QVariant();
}

bool
ActionDockBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    Q_EMIT headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}

int
ActionDockBlockModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

QVariant
ActionDockBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::CheckStateRole ) ) {
    return QVariant();
  }

  if( role == Qt::CheckStateRole ) {
    int countRow = 0;

    const auto& constRefOfList = scene->items();

    for( const auto& item : constRefOfList ) {
      auto* block = qgraphicsitem_cast< QNEBlock* >( item );

      if( block != nullptr ) {
        if( auto* object = qobject_cast< ActionDockBlock* >( block->objects.front() ) ) {
          if( countRow++ == index.row() ) {
            switch( index.column() ) {
              case 1:
                return object->widget->state() ? Qt::Checked : Qt::Unchecked;

              case 2:
                return object->widget->isCheckable() ? Qt::Checked : Qt::Unchecked;
            }
          }
        }
      }
    }
  }

  if( role == Qt::DisplayRole || role == Qt::EditRole ) {
    int countRow = 0;

    const auto& constRefOfList = scene->items();

    for( const auto& item : constRefOfList ) {
      auto* block = qgraphicsitem_cast< QNEBlock* >( item );

      if( block != nullptr ) {
        if( auto* object = qobject_cast< ActionDockBlock* >( block->objects.front() ) ) {
          if( countRow++ == index.row() ) {
            switch( index.column() ) {
              case 0:
                return block->getName();

              case 3:
                return object->widget->getTheme();
            }
          }
        }
      }
    }
  }

  return QVariant();
}

bool
ActionDockBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( auto* object = qobject_cast< ActionDockBlock* >( block->objects.front() ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast< QString >( value ) );
              Q_EMIT dataChanged( index, index, QVector< int >() << role );
              return true;

            case 1:
              object->widget->setState( value.toBool() );
              Q_EMIT dataChanged( index, index, QVector< int >() << role );
              return true;

            case 2:
              object->widget->setCheckable( value.toBool() );
              Q_EMIT dataChanged( index, index, QVector< int >() << role );
              return true;

            case 3:
              object->widget->setTheme( value.toString() );
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
ActionDockBlockModel::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( QStringLiteral( "Action/State Dock" ), QVariant::fromValue( this ) );
}

void
ActionDockBlockModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( qobject_cast< ActionDockBlock* >( block->objects.front() ) != nullptr ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}
