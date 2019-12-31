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

#include "../block/Implement.h"

#include "MeterModel.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "../qnodeseditor/qneblock.h"

#include "../block/MeterBarModel.h"

MeterModel::MeterModel( QGraphicsScene* scene )
  : QAbstractTableModel(),
    scene( scene ) {
}

QVariant MeterModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      case 1:
        return QStringLiteral( "Name Visible" );

      case 2:
        return QStringLiteral( "Precision" );

      case 3:
        return QStringLiteral( "Scale" );

      case 4:
        return QStringLiteral( "Field Width" );

      case 5:
        return QStringLiteral( "Font Family" );

      case 6:
        return QStringLiteral( "Font Size" );

      case 7:
        return QStringLiteral( "Bold Font" );

      default:
        return QString();
    }
  }

  return QVariant();
}

Qt::ItemFlags MeterModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 1 || index.column() == 7 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  } else {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
  }
}

bool MeterModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    emit headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}


int MeterModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

int MeterModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 8;
}

QVariant MeterModel::data( const QModelIndex& index, int role ) const {

  if( index.isValid() ) {

    if( role == Qt::CheckStateRole ) {
      int countRow = 0;

      for( auto item : scene->items() ) {
        auto* block = qgraphicsitem_cast<QNEBlock*>( item );

        if( block ) {
          if( auto* object = qobject_cast<MeterBarModel*>( block->object ) ) {
            if( countRow++ == index.row() ) {
              switch( index.column() ) {
                case 1:
                  return object->widget->captionEnabled() ? Qt::Checked : Qt::Unchecked;

                case 7:
                  return object->widget->fontOfLabel().bold() ? Qt::Checked : Qt::Unchecked;
              }
            }
          }
        }
      }
    }

    if( role == Qt::DisplayRole || role == Qt::EditRole ) {

      int countRow = 0;

      for( auto item : scene->items() ) {
        auto* block = qgraphicsitem_cast<QNEBlock*>( item );

        if( block ) {
          if( auto* object = qobject_cast<MeterBarModel*>( block->object ) ) {
            if( countRow++ == index.row() ) {
              switch( index.column() ) {
                case 0:
                  return block->getName();

//                case 1:
//                  return object->widget->captionEnabled();

                case 2:
                  return object->widget->precision;

                case 3:
                  return object->widget->scale;

                case 4:
                  return object->widget->fieldWidth;

                case 5:
                  return object->widget->fontOfLabel();

                case 6:
                  return object->widget->fontOfLabel().pointSize();

//                case 7:
//                  return object->widget->fontOfLabel().bold();
              }
            }
          }
        }
      }
    }
  }

  return QVariant();
}

bool MeterModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  for( auto item : scene->items() ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( auto* object = qobject_cast<MeterBarModel*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast<QString>( value ) );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 1:
              object->widget->setCaptionEnabled( value.toBool() );
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 2:
              object->widget->precision = value.toString().toInt();
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 3:
              object->widget->scale = value.toString().toFloat();
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 4:
              object->widget->fieldWidth = value.toString().toInt();
              emit dataChanged( index, index, QVector<int>() << role );
              return true;

            case 5: {
                QFont font = value.value<QFont>();
                QFont oldFont = object->widget->fontOfLabel();
                font.setBold( oldFont.bold() );
                font.setPointSize( oldFont.pointSize() );
                object->widget->setFontOfLabel( font );
                emit dataChanged( index, index, QVector<int>() << role );
                return true;
              }

            case 6: {
                QFont font = object->widget->fontOfLabel();
                font.setPointSize( value.toInt() );
                object->widget->setFontOfLabel( font );
                emit dataChanged( index, index, QVector<int>() << role );
                return true;
              }

            case 7: {
                QFont font = object->widget->fontOfLabel();
                font.setBold( value.toBool() );
                object->widget->setFontOfLabel( font );
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

void MeterModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  for( auto item : scene->items() ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( qobject_cast<MeterBarModel*>( block->object ) ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}
