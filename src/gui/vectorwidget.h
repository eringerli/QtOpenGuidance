#ifndef VECTORWIDGET_H
#define VECTORWIDGET_H

#include <QWidget>
#include <QQuaternion>
#include <QVector3D>

#include "../GuidanceBase.h"

namespace Ui {
  class VectorWidget;
}

class VectorWidget : public QWidget {
    Q_OBJECT

  public:
    explicit VectorWidget( QWidget* parent = nullptr );
    ~VectorWidget();

  private slots:
    void on_sbX_valueChanged( double arg1 );

    void on_sbY_valueChanged( double arg1 );

    void on_sbZ_valueChanged( double arg1 );

  signals:
    void vectorChanged( QVector3D );

  private:
    Ui::VectorWidget* ui;

  public:
    QVector3D vector;
};



class VectorFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    VectorFactory()
      : GuidanceFactory() {}
    ~VectorFactory() {}

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( QStringLiteral( "Vector3D Widget" ), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return nullptr;
    }

    virtual void createBlock( QGraphicsScene* scene, QObject* ) override {
      QWidget* widget = new VectorWidget();

      QNEBlock* b = new QNEBlock( widget );
      scene->addItem( b );

      b->addPort( "Vector3D", "", 0, QNEPort::NamePort );
      b->addPort( "Vector3D Widget", "", 0, QNEPort::TypePort );

      b->addWidget( widget );

      b->addOutputPort( "Position", SIGNAL( vectorChanged( QVector3D ) ) );
    }
};

#endif // VECTORWIDGET_H
