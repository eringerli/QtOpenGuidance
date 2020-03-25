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

#include "../block/Implement.h"

#include "ValueBlockModel.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "../qnodeseditor/qneblock.h"

#include "../block/ValueDockBlockBase.h"

ValueBlockModel::ValueBlockModel( QGraphicsScene* scene )
  : scene( scene ) {
}

QVariant ValueBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
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

      default:
        return QString();
    }
  }

  return QVariant();
}

Qt::ItemFlags ValueBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 1 || index.column() == 8 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

}

bool ValueBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    emit headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}


int ValueBlockModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

int ValueBlockModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 9;
}

QVariant ValueBlockModel::data( const QModelIndex& index, int role ) const {

  if( index.isValid() ) {
    if( role == Qt::CheckStateRole ) {
      int countRow = 0;

      const auto& constRefOfList = scene->items();

      for( const auto& item : constRefOfList ) {
        auto* block = qgraphicsitem_cast<QNEBlock*>( item );

        if( block != nullptr ) {
          if( auto* object = qobject_cast<ValueDockBlockBase*>( block->object ) ) {
            if( countRow++ == index.row() ) {
              switch( index.column() ) {
                case 1:
                  return object->unitVisible() ? Qt::Checked : Qt::Unchecked;

                case 8:
                  return object->getFont().bold() ? Qt::Checked : Qt::Unchecked;
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
        auto* block = qgraphicsitem_cast<QNEBlock*>( item );

        if( block != nullptr ) {
          if( auto* object = qobject_cast<ValueDockBlockBase*>( block->object ) ) {
            if( countRow++ == index.row() ) {
              switch( index.column() ) {
                case 0:
                  return block->getName();

                case 2:
                  return object->getUnit();

                case 3:
                  return object->getPrecision();

                case 4:
                  return object->getScale();

                case 5:
                  return object->getFieldWidth();

                case 6:
                  return object->getFont();

                case 7:
                  return object->getFont().pointSize();
              }
            }
          }
        }
      }
    }
  }

  return QVariant();
}

bool ValueBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( auto* object = qobject_cast<ValueDockBlockBase*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast<QString>( value ) );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 1:
              object->setUnitVisible( value.toBool() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 2:
              object->setUnit( qvariant_cast<QString>( value ) );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 3:
              object->setPrecision( value.toString().toInt() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 4:
              object->setScale( value.toString().toFloat() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 5:
              object->setFieldWidth( value.toString().toInt() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 6: {
                QFont font = value.value<QFont>();
                QFont oldFont = object->getFont();
                font.setBold( oldFont.bold() );
                font.setPointSize( oldFont.pointSize() );
                object->setFont( font );
                emit dataChanged( index, index, QVector<int>() << role );
                return true;
              }

            case 7: {
                QFont font = object->getFont();
                font.setPointSize( value.toInt() );
                object->setFont( font );
                emit dataChanged( index, index, QVector<int>() << role );
                return true;
              }

            case 8: {
                QFont font = object->getFont();
                font.setBold( value.toBool() );
                object->setFont( font );
                emit dataChanged( index, index, QVector<int>() << role );
                return true;
              }
          }
        }
      }
    }
  }

  return false;
}

void ValueBlockModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( qobject_cast<ValueDockBlockBase*>( block->object ) != nullptr ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}
