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

#include "../gui/ImplementBlockModel.h"

#ifndef IMPLEMENT_H
#define IMPLEMENT_H

#include <QObject>

#include "BlockBase.h"

class Implement : public BlockBase {
    Q_OBJECT

  public:
    explicit Implement()
      : BlockBase() {}

    void emitConfigSignals() override {
      emit stringChanged( string );
    }

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;
      valuesObject["String"] = string;
      json["values"] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json["values"].isObject() ) {
        QJsonObject valuesObject = json["values"].toObject();

        if( valuesObject["String"].isString() ) {
          string = valuesObject["String"].toString();
        }
      }
    }

  signals:
    void stringChanged( QString );

  public:
    QString string;
};

class ImplementFactory : public BlockFactory {
    Q_OBJECT

  public:
    ImplementFactory( ImplementBlockModel* model )
      : BlockFactory(),
        model( model ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Implement" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new Implement();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addOutputPort( "String", SIGNAL( stringChanged( QString ) ) );

      model->resetModel();

      return b;
    }

  private:
    ImplementBlockModel* model = nullptr;
};

#endif // IMPLEMENT_H
