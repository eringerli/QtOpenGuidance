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

#include "LengthBlockModel.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "../qnodeseditor/qneblock.h"

LengthBlockModel::LengthBlockModel( QGraphicsScene* scene )
  : QAbstractTableModel(),
    scene( scene ) {
}

QVariant LengthBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );
        break;

      case 1:
        return QStringLiteral( "Length" );
        break;

      default:
        return QStringLiteral( "" );
        break;
    }
  }

  return QVariant();
}

bool LengthBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    // FIXME: Implement me!
    emit headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}


int LengthBlockModel::rowCount( const QModelIndex& parent ) const {
  int count = 0;

  foreach( QGraphicsItem* item, scene->items() ) {
    QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( qobject_cast<LengthObject*>( block->object ) ) {
        count++;
      }
    }
  }

  return count;
}

int LengthBlockModel::columnCount( const QModelIndex& parent ) const {
  return 2;
}

QVariant LengthBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole &&  role != Qt::EditRole ) ) {
    return QVariant();
  }

  int countRow = 0;

  foreach( QGraphicsItem* item, scene->items() ) {
    QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( LengthObject* object = qobject_cast<LengthObject*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              return block->getName();

            case 1:
              return object->length;
          }
        }
      }
    }
  }

  return QVariant();
}

bool LengthBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  foreach( QGraphicsItem* item, scene->items() ) {
    QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( LengthObject* object = qobject_cast<LengthObject*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast<QString>( value ) );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 1:
              object->length= value.toString().toFloat();
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

Qt::ItemFlags LengthBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void LengthBlockModel::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( QStringLiteral( "Lengths" ), QVariant::fromValue( this ) );
}

void LengthBlockModel::resetModel() {
  beginResetModel();
  endResetModel();
}
