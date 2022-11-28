// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
