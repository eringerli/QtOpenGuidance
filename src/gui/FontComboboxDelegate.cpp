// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FontComboboxDelegate.h"

#include <QFontComboBox>

FontComboboxDelegate::FontComboboxDelegate( QObject* parent ) : QStyledItemDelegate( parent ) {}

QWidget*
FontComboboxDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& ) const {
  QFontComboBox* editor = new QFontComboBox( parent );
  editor->setFrame( false );
  return editor;
}

void
FontComboboxDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
  QFont value = index.model()->data( index, Qt::EditRole ).value< QFont >();

  QFontComboBox* fontComboBox = static_cast< QFontComboBox* >( editor );
  fontComboBox->setCurrentFont( value );
}

void
FontComboboxDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
  QFontComboBox* fontComboBox = static_cast< QFontComboBox* >( editor );
  fontComboBox->currentFont().toString();
  QFont value = fontComboBox->currentFont();

  model->setData( index, value, Qt::EditRole );
}

void
FontComboboxDelegate::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& ) const {
  editor->setGeometry( option.rect );
}
