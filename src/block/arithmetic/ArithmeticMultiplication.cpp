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

#include "ArithmeticMultiplication.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

QNEBlock* ArithmeticMultiplicationFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ArithmeticMultiplication();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "A" ), QLatin1String( SLOT( setValueA( double ) ) ) );
  b->addPort( QStringLiteral( "*" ), QLatin1String(), false, QNEPort::NoBullet );
  b->addInputPort( QStringLiteral( "B" ), QLatin1String( SLOT( setValueB( double ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( numberChanged( double ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}

void ArithmeticMultiplication::emitConfigSignals() {
  Q_EMIT numberChanged( result );
}

void ArithmeticMultiplication::setValueA( double number ) {
  numberA = number;
  operation();
}

void ArithmeticMultiplication::setValueB( double number ) {
  numberB = number;
  operation();
}

void ArithmeticMultiplication::operation() {
  result = numberA * numberB;
  Q_EMIT numberChanged( result );
}
