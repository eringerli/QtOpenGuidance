// Copyright( C ) 2020 Christian Riggenbach
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

#include "TransmissionBlockModel.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qneblock.h"

#include "block/converter/ValueTransmissionBase.h"
#include "block/converter/ValueTransmissionBase64Data.h"
#include "block/converter/ValueTransmissionNumber.h"
#include "block/converter/ValueTransmissionQuaternion.h"

TransmissionBlockModel::TransmissionBlockModel( QGraphicsScene* scene )
  : scene( scene ) {
}

QVariant TransmissionBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
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

      default:
        return QString();
    }
  }

  return QVariant();
}

Qt::ItemFlags TransmissionBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 1 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

}

bool TransmissionBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    Q_EMIT headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}


int TransmissionBlockModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

int TransmissionBlockModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 5;
}

QVariant TransmissionBlockModel::data( const QModelIndex& index, int role ) const {

  if( index.isValid() ) {
    if( role == Qt::DisplayRole || role == Qt::EditRole ) {

      int countRow = 0;

      const auto& constRefOfList = scene->items();

      for( const auto& item : constRefOfList ) {
        auto* block = qgraphicsitem_cast<QNEBlock*>( item );

        if( block != nullptr ) {
          if( auto* object = qobject_cast<ValueTransmissionBase*>( block->object ) ) {
            if( countRow++ == index.row() ) {
              switch( index.column() ) {
                case 0:
                  return block->getName();

                case 1: {
                  if( qobject_cast<ValueTransmissionBase64Data*>( object ) != nullptr ) {
                    return QStringLiteral( "Data" );
                  }

                  if( qobject_cast<ValueTransmissionNumber*>( object ) != nullptr ) {
                    return QStringLiteral( "Number" );
                  }

                  if( qobject_cast<ValueTransmissionQuaternion*>( object ) != nullptr ) {
                    return QStringLiteral( "Quaternion" );
                  }

                  return QStringLiteral( "Unknown" );
                }

                case 2:
                  return object->id;

                case 3:
                  return object->timeoutTimeMs;

                case 4:
                  return object->repeatTimeMs;
              }
            }
          }
        }
      }
    }
  }

  return QVariant();
}

bool TransmissionBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( auto* object = qobject_cast<ValueTransmissionBase*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast<QString>( value ) );
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;

            case 2:
              object->id = value.toString().toInt();
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;

            case 3:
              object->timeoutTimeMs = value.toString().toInt();
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;

            case 4:
              object->repeatTimeMs = value.toString().toInt();
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;

            default:
              break;
          }
        }
      }
    }
  }

  return false;
}

void TransmissionBlockModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( qobject_cast<ValueTransmissionBase*>( block->object ) != nullptr ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}
