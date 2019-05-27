#include "VectorBlockModel.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "../qnodeseditor/qneblock.h"

VectorBlockModel::VectorBlockModel( QGraphicsScene* scene )
  : QAbstractTableModel(),
    scene( scene ) {
}

QVariant VectorBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );
        break;

      case 1:
        return QStringLiteral( "X" );
        break;

      case 2:
        return QStringLiteral( "Y" );
        break;

      case 3:
        return QStringLiteral( "Z" );
        break;

      default:
        return QStringLiteral( "" );
        break;
    }
  }

  return QVariant();
}

bool VectorBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    // FIXME: Implement me!
    emit headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}


int VectorBlockModel::rowCount( const QModelIndex& parent ) const {
  int count = 0;

  foreach( QGraphicsItem* item, scene->items() ) {
    QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( qobject_cast<VectorObject*>( block->object ) ) {
        count++;
      }
    }
  }

  return count;
}

int VectorBlockModel::columnCount( const QModelIndex& parent ) const {
  return 4;
}

QVariant VectorBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole &&  role != Qt::EditRole ) ) {
    return QVariant();
  }

  int countRow = 0;

  foreach( QGraphicsItem* item, scene->items() ) {
    QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( VectorObject* object = qobject_cast<VectorObject*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              return block->getName();

            case 1:
              return object->vector.x();

            case 2:
              return object->vector.y();

            case 3:
              return object->vector.z();
          }
        }
      }
    }
  }

  return QVariant();
}

bool VectorBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  foreach( QGraphicsItem* item, scene->items() ) {
    QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( VectorObject* object = qobject_cast<VectorObject*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast<QString>( value ) );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 1:
              object->vector.setX( value.toString().toFloat() );
              object->emitConfigSignals();
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 2:
              object->vector.setY( value.toString().toFloat() );
              object->emitConfigSignals();
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 3:
              object->vector.setZ( value.toString().toFloat() );
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

Qt::ItemFlags VectorBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void VectorBlockModel::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( QStringLiteral( "Vectors" ), QVariant::fromValue( this ) );
}

void VectorBlockModel::resetModel() {
  beginResetModel();
  endResetModel();
}
