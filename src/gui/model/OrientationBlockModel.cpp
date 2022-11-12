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

#include "OrientationBlockModel.h"

#include "block/literal/OrientationBlock.h"

#include <QComboBox>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qneblock.h"

#include "helpers/anglesHelper.h"
#include "helpers/eigenHelper.h"

OrientationBlockModel::OrientationBlockModel( QGraphicsScene* scene ) : scene( scene ) {}

QVariant
OrientationBlockModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  if( role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal ) {
    switch( section ) {
      case 0:
        return QStringLiteral( "Name" );

      case 1:
        return QStringLiteral( "Yaw" );

      case 2:
        return QStringLiteral( "Pitch" );

      case 3:
        return QStringLiteral( "Roll" );

      case 4:
        return QStringLiteral( "x" );

      case 5:
        return QStringLiteral( "y" );

      case 6:
        return QStringLiteral( "z" );

      case 7:
        return QStringLiteral( "w" );

      default:
        return QString();
    }
  }

  return QVariant();
}

bool
OrientationBlockModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role ) {
  if( value != headerData( section, orientation, role ) ) {
    // FIXME: Implement me!
    Q_EMIT headerDataChanged( orientation, section, section );
    return true;
  }

  return false;
}

int
OrientationBlockModel::rowCount( const QModelIndex& /*parent*/ ) const {
  return countBuffer;
}

int
OrientationBlockModel::columnCount( const QModelIndex& /*parent*/ ) const {
  return 8;
}

QVariant
OrientationBlockModel::data( const QModelIndex& index, int role ) const {
  if( !index.isValid() || ( role != Qt::DisplayRole && role != Qt::EditRole ) ) {
    return QVariant();
  }

  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( auto* object = qobject_cast< OrientationBlock* >( block->object ) ) {
        if( countRow++ == index.row() ) {
          switch( index.column() ) {
            case 0:
              return block->getName();

            case 1:
              return radiansToDegrees( getYaw( quaternionToTaitBryan( object->orientation ) ) );

            case 2:
              return radiansToDegrees( getPitch( quaternionToTaitBryan( object->orientation ) ) );

            case 3:
              return radiansToDegrees( getRoll( quaternionToTaitBryan( object->orientation ) ) );

            case 4:
              return object->orientation.x();

            case 5:
              return object->orientation.y();

            case 6:
              return object->orientation.z();

            case 7:
              return object->orientation.w();
          }
        }
      }
    }
  }

  return QVariant();
}

bool
OrientationBlockModel::setData( const QModelIndex& index, const QVariant& value, int role ) {
  int countRow = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( auto* object = qobject_cast< OrientationBlock* >( block->object ) ) {
        if( countRow++ == index.row() ) {
          auto taitBryan = quaternionToTaitBryan( object->orientation );

          switch( index.column() ) {
            case 0:
              block->setName( qvariant_cast< QString >( value ) );
              Q_EMIT dataChanged( index, index, QVector< int >() << role );
              return true;

            case 1:
              object->orientation =
                taitBryanToQuaternion( degreesToRadians( value.toString().toDouble() ), getPitch( taitBryan ), getRoll( taitBryan ) );
              object->emitConfigSignals();
              Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
              return true;

            case 2:
              object->orientation =
                taitBryanToQuaternion( getYaw( taitBryan ), degreesToRadians( value.toString().toDouble() ), getRoll( taitBryan ) );
              object->emitConfigSignals();
              Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
              return true;

            case 3:
              object->orientation =
                taitBryanToQuaternion( getYaw( taitBryan ), getPitch( taitBryan ), degreesToRadians( value.toString().toDouble() ) );
              object->emitConfigSignals();
              Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
              return true;

            case 4:
              object->orientation.x() = value.toString().toDouble();
              object->orientation.normalize();
              object->emitConfigSignals();
              Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
              return true;

            case 5:
              object->orientation.y() = value.toString().toDouble();
              object->orientation.normalize();
              object->emitConfigSignals();
              Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
              return true;

            case 6:
              object->orientation.z() = value.toString().toDouble();
              object->orientation.normalize();
              object->emitConfigSignals();
              Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
              return true;

            case 7:
              object->orientation.w() = value.toString().toDouble();
              object->orientation.normalize();
              object->emitConfigSignals();
              Q_EMIT dataChanged( index.siblingAtColumn( 1 ), index.siblingAtColumn( 7 ), QVector< int >() << role );
              return true;
          }
        }
      }
    }
  }

  return false;
}

Qt::ItemFlags
OrientationBlockModel::flags( const QModelIndex& index ) const {
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void
OrientationBlockModel::addToCombobox( QComboBox* combobox ) {
  combobox->addItem( QStringLiteral( "Orientation" ), QVariant::fromValue( this ) );
}

void
OrientationBlockModel::resetModel() {
  beginResetModel();
  countBuffer = 0;

  const auto& constRefOfList = scene->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( qobject_cast< OrientationBlock* >( block->object ) != nullptr ) {
        ++countBuffer;
      }
    }
  }

  endResetModel();
}
