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
      valuesObject[QStringLiteral( "X" )] = double( vector.x() );
      valuesObject[QStringLiteral( "Y" )] = double( vector.y() );
      valuesObject[QStringLiteral( "Z" )] = double( vector.z() );
      json[QStringLiteral( "values" )] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        if( valuesObject[QStringLiteral( "X" )].isDouble() ) {
          vector.setX( float( valuesObject[QStringLiteral( "X" )].toDouble() ) );
        }

        if( valuesObject[QStringLiteral( "Y" )].isDouble() ) {
          vector.setY( float( valuesObject[QStringLiteral( "Y" )].toDouble() ) );
        }

        if( valuesObject[QStringLiteral( "Z" )].isDouble() ) {
          vector.setZ( float( valuesObject[QStringLiteral( "Z" )].toDouble() ) );

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
      auto* b = createBaseBlock( scene, obj );

      b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( vectorChanged( QVector3D ) ) ) );

      model->resetModel();

      return b;
    }

  private:
    VectorBlockModel* model = nullptr;
};

#endif // VECTOROBJECT_H
