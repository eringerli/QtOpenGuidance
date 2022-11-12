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

class ArithmeticDivision : public BlockBase {
  Q_OBJECT

public:
  explicit ArithmeticDivision() : BlockBase() {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setValueA( NUMBER_SIGNATURE_SLOT );
  void setValueB( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  void operation();

public:
  double numberA = 1;
  double numberB = 1;
  double result  = 0;
};

class ArithmeticDivisionFactory : public BlockFactory {
  Q_OBJECT

public:
  ArithmeticDivisionFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ArithmeticDivision" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Arithmetic Division" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Arithmetic" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
