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

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class NumberBlockModel;

class NumberObject : public BlockBase {
    Q_OBJECT

  public:
    explicit NumberObject()
      : BlockBase() {}

    void emitConfigSignals() override;

    void toJSON( QJsonObject& json ) override;
    void fromJSON( QJsonObject& json ) override;

  Q_SIGNALS:
    void numberChanged( const double );

  public:
    double number = 0;
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

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Literals" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

  private:
    NumberBlockModel* model = nullptr;
};
