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

#include "../gui/NumberBlockModel.h"

#pragma once

#include <QObject>

#include "BlockBase.h"

class NumberObject : public BlockBase {
    Q_OBJECT

  public:
    explicit NumberObject()
      : BlockBase() {}

    void emitConfigSignals() override {
      emit numberChanged( number );
    }

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;
      valuesObject[QStringLiteral( "Number" )] = double( number );
      json[QStringLiteral( "values" )] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        if( valuesObject[QStringLiteral( "Number" )].isDouble() ) {
          number = float( valuesObject[QStringLiteral( "Number" )].toDouble() );
        }
      }
    }

  signals:
    void numberChanged( double );

  public:
    float number = 0;
};

class NumberFactory : public BlockFactory {
    Q_OBJECT

  public:
    NumberFactory( NumberBlockModel* model )
      : BlockFactory(),
        model( model ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Number" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new NumberObject();
      auto* b = createBaseBlock( scene, obj, id );

      b->addOutputPort( QStringLiteral( "Number" ), QLatin1String( SIGNAL( numberChanged( double ) ) ) );

      model->resetModel();

      return b;
    }

  private:
    NumberBlockModel* model = nullptr;
};

