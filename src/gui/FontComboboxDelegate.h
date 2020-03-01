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

#include <QStyledItemDelegate>

#include <QFontComboBox>

class FontComboboxDelegate : public QStyledItemDelegate {
    Q_OBJECT

  public:
    FontComboboxDelegate( QObject* parent = nullptr )
      : QStyledItemDelegate( parent )
    {}

    QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem&,
                           const QModelIndex& ) const override {
      QFontComboBox* editor = new QFontComboBox( parent );
      editor->setFrame( false );
      return editor;
    }

    void setEditorData( QWidget* editor, const QModelIndex& index ) const override {
      QFont value = index.model()->data( index, Qt::EditRole ).value<QFont>();

      QFontComboBox* fontComboBox = static_cast<QFontComboBox*>( editor );
      fontComboBox->setCurrentFont( value );
    }

    void setModelData( QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index ) const override {
      QFontComboBox* fontComboBox = static_cast<QFontComboBox*>( editor );
      fontComboBox->currentFont().toString();
      QFont value = fontComboBox->currentFont();

      model->setData( index, value, Qt::EditRole );
    }

    void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option,
                               const QModelIndex& ) const override {
      editor->setGeometry( option.rect );
    }
};

