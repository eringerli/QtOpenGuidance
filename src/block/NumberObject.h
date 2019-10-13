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

#include "../gui/NumberBlockModel.h"

#ifndef NUMBEROBJECT_H
#define NUMBEROBJECT_H

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
      valuesObject["Number"] = double( number );
      json["values"] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json["values"].isObject() ) {
        QJsonObject valuesObject = json["values"].toObject();

        if( valuesObject["Number"].isDouble() ) {
          number = float( valuesObject["Number"].toDouble() );
        }
      }
    }

  signals:
    void numberChanged( float );

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

    virtual BlockBase* createNewObject() override {
      return new NumberObject();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addOutputPort( "Number", SIGNAL( numberChanged( float ) ) );

      model->resetModel();

      return b;
    }

  private:
    NumberBlockModel* model = nullptr;
};

#endif // NUMBEROBJECT_H
