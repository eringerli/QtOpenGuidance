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

#include "PathPlannerModelBlockModel.h"

#include "block/graphical/PathPlannerModel.h"

#include <QComboBox>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qneblock.h"

PathPlannerModelBlockModel::PathPlannerModelBlockModel( QGraphicsScene* scene )
  : scene( scene ) {
}

QVariant PathPlannerModelBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      default:
        return QString();
    }
  }

  return QVariant();
}

bool PathPlannerModelBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    // FIXME: Implement me!
    Q_EMIT headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}


int PathPlannerModelBlockModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

int PathPlannerModelBlockModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 2;
}

QVariant PathPlannerModelBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
    return QVariant();
  }

  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( qobject_cast<PathPlannerModel*>( block->object ) != nullptr ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              return block->getName();

            case 1:
              return QVariant::fromValue( block );
          }
        }
      }
    }
  }

  return QVariant();
}

bool PathPlannerModelBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( /*auto* object = */qobject_cast<PathPlannerModel*>( block->object ) != nullptr ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast<QString>( value ) );
              Q_EMIT dataChanged( index, index, QVector<int>() << role );
              return true;
          }
        }
      }
    }
  }

  return false;
}

Qt::ItemFlags PathPlannerModelBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void PathPlannerModelBlockModel::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( QStringLiteral( "PathPlannerModel" ), QVariant::fromValue( this ) );
}

void PathPlannerModelBlockModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( qobject_cast<PathPlannerModel*>( block->object ) != nullptr ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}
