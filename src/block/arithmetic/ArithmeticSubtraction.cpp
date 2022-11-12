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

#include "ArithmeticSubtraction.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
ArithmeticSubtraction::emitConfigSignals() {
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

void
ArithmeticSubtraction::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ArithmeticSubtraction::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ArithmeticSubtraction::operation() {
  result = numberA - numberB;
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

QNEBlock*
ArithmeticSubtractionFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ArithmeticSubtraction();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "A" ), QLatin1String( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  b->addPort( QStringLiteral( "-" ), QLatin1String(), false, QNEPort::NoBullet );
  b->addInputPort( QStringLiteral( "B" ), QLatin1String( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}
