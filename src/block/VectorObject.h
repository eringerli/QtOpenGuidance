// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef VECTOROBJECT_H
#define VECTOROBJECT_H

#include <QObject>
#include <QQuaternion>
#include <QVector3D>

#include "GuidanceBase.h"

class VectorObject : public GuidanceBase {
    Q_OBJECT

  public:
    explicit VectorObject()
      : vector( QVector3D( 0, 0, 0 ) ) {}

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

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( QStringLiteral( "Vector3D" ), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new VectorObject();
    }

    virtual void createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( "Vector3D", "", 0, QNEPort::NamePort );
      b->addPort( "Vector3D", "", 0, QNEPort::TypePort );

      b->addOutputPort( "Position", SIGNAL( vectorChanged( QVector3D ) ) );
    }
};

#endif // VECTOROBJECT_H
