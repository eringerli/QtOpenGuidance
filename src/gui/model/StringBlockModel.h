// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QtCore/QAbstractTableModel>

class BlocksManager;

class StringBlockModel : public QAbstractTableModel {
  Q_OBJECT

public:
  QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

  int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
  int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

  QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;

  bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) override;

  Qt::ItemFlags flags( const QModelIndex& index ) const override;

public Q_SLOTS:
  void resetModel();

private:
  int countBuffer = 0;
};
