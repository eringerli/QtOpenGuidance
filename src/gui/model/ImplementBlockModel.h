// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QtCore/QAbstractTableModel>

class BlocksManager;

class ImplementBlockModel : public QAbstractTableModel {
  Q_OBJECT

public:
  explicit ImplementBlockModel( BlocksManager* blocksManager );

  QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

  int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
  int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

  QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;

  bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) override;

  Qt::ItemFlags flags( const QModelIndex& index ) const override;

public Q_SLOTS:
  void resetModel();

private:
  BlocksManager* blocksManager = nullptr;

  int countBuffer = 0;
};
