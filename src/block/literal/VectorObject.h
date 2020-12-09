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

#include "../gui/model/VectorBlockModel.h"

#pragma once

#include <QObject>

#include <Eigen/Geometry>
#include <QVector3D>

#include "../BlockBase.h"

class VectorObject : public BlockBase {
    Q_OBJECT

  public:
    explicit VectorObject()
      : vector( Eigen::Vector3d( 0, 0, 0 ) ) {}

    void emitConfigSignals() override {
      Q_EMIT vectorChanged( vector );
    }

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;
      valuesObject[QStringLiteral( "X" )] = vector.x();
      valuesObject[QStringLiteral( "Y" )] = vector.y();
      valuesObject[QStringLiteral( "Z" )] = vector.z();
      json[QStringLiteral( "values" )] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        if( valuesObject[QStringLiteral( "X" )].isDouble() ) {
          vector.x() = valuesObject[QStringLiteral( "X" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "Y" )].isDouble() ) {
          vector.y() = valuesObject[QStringLiteral( "Y" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "Z" )].isDouble() ) {
          vector.z() = valuesObject[QStringLiteral( "Z" )].toDouble();
        }
      }
    }

  Q_SIGNALS:
    void vectorChanged( const Eigen::Vector3d& );

  public:
    Eigen::Vector3d vector;
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

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Literals" );
    }

    QString getPrettyNameOfFactory() override {
      return QStringLiteral( "3D Vector" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new VectorObject();
      auto* b = createBaseBlock( scene, obj, id );

      b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( vectorChanged( const Eigen::Vector3d& ) ) ) );

      b->setBrush( valueColor );

      model->resetModel();

      return b;
    }

  private:
    VectorBlockModel* model = nullptr;
};
