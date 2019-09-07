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

#include "../gui/VectorBlockModel.h"

#ifndef VECTOROBJECT_H
#define VECTOROBJECT_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

class VectorObject : public BlockBase {
    Q_OBJECT

  public:
    explicit VectorObject()
      : vector( QVector3D( 0, 0, 0 ) ) {}

    void emitConfigSignals() override {
      emit vectorChanged( vector );
    }

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;
      valuesObject["X"] = double( vector.x() );
      valuesObject["Y"] = double( vector.y() );
      valuesObject["Z"] = double( vector.z() );
      json["values"] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json["values"].isObject() ) {
        QJsonObject valuesObject = json["values"].toObject();

        if( valuesObject["X"].isDouble() ) {
          vector.setX( float( valuesObject["X"].toDouble() ) );
        }

        if( valuesObject["Y"].isDouble() ) {
          vector.setY( float( valuesObject["Y"].toDouble() ) );
        }

        if( valuesObject["Z"].isDouble() ) {
          vector.setZ( float( valuesObject["Z"].toDouble() ) );

        }
      }
    }

  signals:
    void vectorChanged( QVector3D );

  public:
    QVector3D vector;
};



class VectorFactory : public BlockFactory {
    Q_OBJECT

  public:
    VectorFactory( VectorBlockModel* model )
      : BlockFactory(),
        model( model ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Vector3D" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new VectorObject();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addOutputPort( "Position", SIGNAL( vectorChanged( QVector3D ) ) );

      model->resetModel();

      return b;
    }

  private:
    VectorBlockModel* model = nullptr;
};

#endif // VECTOROBJECT_H
