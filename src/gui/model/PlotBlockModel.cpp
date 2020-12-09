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

#include "PlotBlockModel.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "../qnodeseditor/qneblock.h"

#include "../../block/dock/plot/PlotDockBlockBase.h"

PlotBlockModel::PlotBlockModel( QGraphicsScene* scene )
  : scene( scene ) {
}

QVariant PlotBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      case 1:
        return QStringLiteral( "X Axis Visible" );

      case 2:
        return QStringLiteral( "Y Axis Visible" );

      case 3:
        return QStringLiteral( "Y Axis Description" );

      case 4:
        return QStringLiteral( "Autoscroll Enabled" );

      case 5:
        return QStringLiteral( "Window" );

      default:
        return QString();
    }
  }

  return QVariant();
}

Qt::ItemFlags PlotBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  if( index.column() == 1 || index.column() == 2 || index.column() == 4 ) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

}

bool PlotBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    Q_EMIT headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}


int PlotBlockModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

int PlotBlockModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 6;
}

QVariant PlotBlockModel::data( const QModelIndex& index, int role ) const {

  if( index.isValid() ) {
    if( role == Qt::CheckStateRole ) {
      int countRow = 0;

      const auto& constRefOfList = scene->items();

      for( const auto& item : constRefOfList ) {
        auto* block = qgraphicsitem_cast<QNEBlock*>( item );

        if( block != nullptr ) {
          if( auto* object = qobject_cast<PlotDockBlockBase*>( block->object ) ) {
            if( countRow++ == index.row() ) {
              switch( index.column() ) {
                case 1:
                  return object->getXAxisVisible() ? Qt::Checked : Qt::Unchecked;

                case 2:
                  return object->getYAxisVisible() ? Qt::Checked : Qt::Unchecked;

                case 4:
                  return object->getAutoscrollEnabled() ? Qt::Checked : Qt::Unchecked;
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
          if( auto* object = qobject_cast<PlotDockBlockBase*>( block->object ) ) {
            if( countRow++ == index.row() ) {
              switch( index.column() ) {
                case 0:
                  return block->getName();

                case 3:
                  return object->getYAxisDescription();

                case 5:
                  return object->getWindow();
              }
            }
          }
        }
      }
    }
  }

  return QVariant();
}

bool PlotBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( auto* object = qobject_cast<PlotDockBlockBase*>( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast<QString>( value ) );
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;

            case 1:
              object->setXAxisVisible( value.toBool() );
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;

            case 2:
              object->setYAxisVisible( value.toBool() );
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;

            case 3:
              object->setYAxisDescription( value.toString() );
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;

            case 4:
              object->setAutoscrollEnabled( value.toBool() );
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;

            case 5:
              object->setWindow( value.toString().toDouble() );
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;
          }
        }
      }
    }
  }

  return false;
}

void PlotBlockModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( qobject_cast<PlotDockBlockBase*>( block->object ) != nullptr ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}
