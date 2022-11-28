// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QStyledItemDelegate>

class FontComboboxDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  FontComboboxDelegate( QObject* parent = nullptr );

  QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& ) const override;

  void setEditorData( QWidget* editor, const QModelIndex& index ) const override;

  void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const override;

  void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& ) const override;
};
