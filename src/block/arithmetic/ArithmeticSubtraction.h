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

#include "block/BlockBase.h"

class ArithmeticSubtraction : public BlockBase {
    Q_OBJECT

  public:
    explicit ArithmeticSubtraction()
      : BlockBase() {}

    void emitConfigSignals() override;

    void setValueA( double number );
    void setValueB( double number );

  private:
    void operation();

  Q_SIGNALS:
    void numberChanged( double );

  public:
    double numberA = 0;
    double numberB = 0;
    double result = 0;
};

class ArithmeticSubtractionFactory : public BlockFactory {
    Q_OBJECT

  public:
    ArithmeticSubtractionFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "ArithmeticSubtraction" );
    }

    QString getPrettyNameOfFactory() override {
      return QStringLiteral( "Arithmetic Subtraction" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Arithmetic" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
