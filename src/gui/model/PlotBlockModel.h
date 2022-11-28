// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QtCore/QAbstractTableModel>

class QComboBox;
class QGraphicsScene;

class PlotBlockModel : public QAbstractTableModel {
  Q_OBJECT

public:
  explicit PlotBlockModel( QGraphicsScene* scene );

  // Header:
  QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

  bool setHeaderData( int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole ) override;

  // Basic functionality:
  int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
  int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

  QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;

  // Editable:
  bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) override;

  Qt::ItemFlags flags( const QModelIndex& index ) const override;

public Q_SLOTS:
  void resetModel();

private:
  QGraphicsScene* scene = nullptr;

  int countBuffer = 0;
};
