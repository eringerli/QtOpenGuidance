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

#ifndef LENGTHWIDGET_H
#define LENGTHWIDGET_H

#include <QObject>

#include "GuidanceBase.h"

class LengthObject : public GuidanceBase {
    Q_OBJECT

  public:
    explicit LengthObject()
      : GuidanceBase() {}

    void emitConfigSignals() override {
      emit lengthChanged( length );
    }

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;
      valuesObject["length"] = double( length );
      json["values"] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json["values"].isObject() ) {
        QJsonObject valuesObject = json["values"].toObject();

        if( valuesObject["length"].isDouble() ) {
          length = float( valuesObject["length"].toDouble() );
        }
      }
    }

  signals:
    void lengthChanged( float );

  public:
    float length = 0;
};

class LengthFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    LengthFactory()
      : GuidanceFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Length" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new LengthObject();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addOutputPort( "Length", SIGNAL( lengthChanged( float ) ) );

      return b;
    }
};

#endif // LENGTHWIDGET_H
