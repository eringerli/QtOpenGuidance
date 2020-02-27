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

