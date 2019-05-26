#ifndef LENGTHWIDGET_H
#define LENGTHWIDGET_H

#include <QWidget>

#include "../GuidanceBase.h"

namespace Ui {
  class LengthWidget;
}

class LengthWidget : public QWidget {
    Q_OBJECT

  public:
    explicit LengthWidget( QWidget* parent = nullptr );
    ~LengthWidget();

  signals:
    void lengthChanged( float );

  private slots:
    void on_doubleSpinBox_valueChanged( double arg1 );

  private:
    Ui::LengthWidget* ui;
};

class LengthFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    LengthFactory()
      : GuidanceFactory() {}
    ~LengthFactory() {}

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( QStringLiteral( "Length Widget" ), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return nullptr;
    }

    virtual void createBlock( QGraphicsScene* scene, QObject* ) override {
      QWidget* widget = new LengthWidget();

      QNEBlock* b = new QNEBlock( widget );
      scene->addItem( b );

      b->addPort( "Length", "", 0, QNEPort::NamePort );
      b->addPort( "Length Widget", "", 0, QNEPort::TypePort );

      b->addWidget( widget );

      b->addOutputPort( "Length", SIGNAL( lengthChanged( float ) ) );
    }
};

#endif // LENGTHWIDGET_H
