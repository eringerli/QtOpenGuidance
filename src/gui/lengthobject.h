#ifndef LENGTHWIDGET_H
#define LENGTHWIDGET_H

#include <QWidget>

#include "../GuidanceBase.h"

class LengthObject : public GuidanceBase {
    Q_OBJECT

  public:
    explicit LengthObject() {}
    ~LengthObject() {}

  signals:
    void lengthChanged( float );

  public:
    float lenght;
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
      return new LengthObject();
    }

    virtual void createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( "Length", "", 0, QNEPort::NamePort );
      b->addPort( "Length Widget", "", 0, QNEPort::TypePort );

      b->addOutputPort( "Length", SIGNAL( lengthChanged( float ) ) );
    }
};

#endif // LENGTHWIDGET_H
