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

#include "block/sectionControl/Implement.h"

#include "ImplementSectionModel.h"

#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qneblock.h"

QVariant ImplementSectionModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Overlap Left" );

      case 1:
        return QStringLiteral( "Width of Section" );

      case 2:
        return QStringLiteral( "Overlap Right" );

      default:
        return QString();
    }
  } else if( role == Qt::DisplayRole && orientation == Qt::Orientation::Vertical ) {
    return QStringLiteral( "Section " ) + QString::number( section + 1 );
  }

  return QVariant();
}

bool ImplementSectionModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    Q_EMIT headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}


int ImplementSectionModel::rowCount( const QModelIndex& /*parent*/ ) const {
  if( block != nullptr ) {
    auto* implement = qobject_cast<Implement*>( block->object );

    if( implement != nullptr ) {
      return implement->sections.size() - 1;
    }
  }

  return 0;
}

int ImplementSectionModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 3;
}

QVariant ImplementSectionModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
    return QVariant();
  }

  if( block != nullptr ) {
    auto* implement = qobject_cast<Implement*>( block->object );

    if( implement != nullptr ) {
      if( size_t( index.row() ) < ( implement->sections.size() - 1 ) ) {
        switch( index.column() ) {
          case 0:
            return implement->sections[index.row() + 1]->overlapLeft;

          case 1:
            return implement->sections[index.row() + 1]->widthOfSection;

          case 2:
            return implement->sections[index.row() + 1]->overlapRight;

          default:
            return QVariant();
        }
      }
    }
  }

  return QVariant();
}

bool ImplementSectionModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  if( block != nullptr ) {
    auto* implement = qobject_cast<Implement*>( block->object );

    if( implement != nullptr ) {
      if( size_t( index.row() ) < ( implement->sections.size() - 1 ) ) {
        switch( index.column() ) {
          case 0:
            implement->sections[index.row() + 1]->overlapLeft = qvariant_cast<double>( value );
            block->emitConfigSignals();
            Q_EMIT dataChanged( index, index, QVector<int>() << role );
            return true;

          case 1:
            implement->sections[index.row() + 1]->widthOfSection = qvariant_cast<double>( value );
            block->emitConfigSignals();
            Q_EMIT dataChanged( index, index, QVector<int>() << role );
            return true;

          case 2:
            implement->sections[index.row() + 1]->overlapRight = qvariant_cast<double>( value );
            block->emitConfigSignals();
            Q_EMIT dataChanged( index, index, QVector<int>() << role );
            return true;
        }
      }
    }
  }

  return false;
}

bool ImplementSectionModel::insertRows( int row, int count, const QModelIndex& parent ) {
  if( block != nullptr ) {
    auto* implement = qobject_cast<Implement*>( block->object );

    if( implement != nullptr ) {
      beginInsertRows( parent, row, row + ( count - 1 ) );

      for( int i = 0; i < count; ++i ) {
        auto it = implement->sections.begin();
        it += row + 1;

        implement->sections.insert( it, new ImplementSection() );
      }

      endInsertRows();

      block->emitConfigSignals();
      return true;
    }
  }

  return false;
}

bool ImplementSectionModel::removeRows( int row, int count, const QModelIndex& parent ) {
  if( block != nullptr ) {
    auto* implement = qobject_cast<Implement*>( block->object );

    if( ( implement != nullptr ) && size_t( row + 1 ) < implement->sections.size() ) {
      beginRemoveRows( parent, row, row + ( count - 1 ) );
      auto first = implement->sections.begin();
      first += row + 1;
      auto last = first;
      last += count;

      implement->sections.erase( first, last );
      endRemoveRows();

      block->emitConfigSignals();
      return true;
    }
  }

  return false;

}

bool ImplementSectionModel::swapElements( int first, int second ) {

  if( block != nullptr ) {
    auto* implement = qobject_cast<Implement*>( block->object );

    if( implement != nullptr ) {
      if( first < second ) {
        std::swap( first, second );
      }

      beginMoveRows( QModelIndex(), first, first, QModelIndex(), second );

      std::swap( implement->sections[first + 1], implement->sections[second + 1] );

      endMoveRows();

      block->emitConfigSignals();
      return true;
    }
  }

  return false;
}


Qt::ItemFlags ImplementSectionModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void ImplementSectionModel::setDatasource( QNEBlock* block ) {
  beginResetModel();
  this->block = block;
  endResetModel();
}
