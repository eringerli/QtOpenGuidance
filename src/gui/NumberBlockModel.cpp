// Copyright( C ) 2019 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "../block/NumberObject.h"

#include "NumberBlockModel.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "../qnodeseditor/qneblock.h"

NumberBlockModel::NumberBlockModel( QGraphicsScene* scene )
  : QAbstractTableModel(),
    scene( scene ) {
}

QVariant NumberBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
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

bool NumberBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    // FIXME: Implement me!
    emit headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}


int NumberBlockModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

int NumberBlockModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 2;
}

QVariant NumberBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole &&  role != Qt::EditRole ) ) {
    return QVariant();
  }

  int countRow = 0;

  for( auto item : scene->items() ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( auto* object = qobject_cast<NumberObject*>( block->object ) ) {
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

bool NumberBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  for( auto item : scene->items() ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( auto* object = qobject_cast<NumberObject*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast<QString>( value ) );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 1:
              object->number = value.toString().toFloat();
              object->emitConfigSignals();
              emit dataChanged( index, index, QVector<int>() << role );
              return true;
          }
        }
      }
    }
  }

  return false;
}

Qt::ItemFlags NumberBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void NumberBlockModel::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( QStringLiteral( "Number" ), QVariant::fromValue( this ) );
}

void NumberBlockModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  for( auto item : scene->items() ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( qobject_cast<NumberObject*>( block->object ) ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}
