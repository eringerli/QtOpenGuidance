#ifndef VECTOROBJECT_H
#define VECTOROBJECT_H

#include <QObject>
#include <QQuaternion>
#include <QVector3D>

#include "../GuidanceBase.h"

class VectorObject : public GuidanceBase {
    Q_OBJECT

  public:
    explicit VectorObject()
      : vector( QVector3D( 1, 2, 3 ) ) {}
    ~VectorObject() {}

    void emitConfigSignals() override {
      emit vectorChanged( vector );
    }

  signals:
    void vectorChanged( QVector3D );

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
      return new VectorObject();
    }

    virtual void createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( "Vector3D", "", 0, QNEPort::NamePort );
      b->addPort( "Vector3D Widget", "", 0, QNEPort::TypePort );

      b->addOutputPort( "Position", SIGNAL( vectorChanged( QVector3D ) ) );
    }
};

#endif // VECTOROBJECT_H
