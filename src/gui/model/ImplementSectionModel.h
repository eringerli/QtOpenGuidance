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

#pragma once

#include <QObject>

#include <QtCore/QAbstractTableModel>

class QComboBox;
class QGraphicsScene;
class QNEBlock;

class ImplementSectionModel : public QAbstractTableModel {
  Q_OBJECT

public:
  explicit ImplementSectionModel() = default;

  // Header:
  QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

  bool setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole ) override;

  // Basic functionality:
  int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
  int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

  QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;

  // Editable:
  bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) override;

  bool insertRows( int row, int count, const QModelIndex& parent = QModelIndex() ) override;
  bool removeRows( int row, int count, const QModelIndex& parent = QModelIndex() ) override;
  bool swapElements( int first, int second );

  Qt::ItemFlags flags( const QModelIndex& index ) const override;

public:
public Q_SLOTS:
  void setDatasource( QNEBlock* block );

private:
  QNEBlock* block = nullptr;
};
