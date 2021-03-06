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

#include "ArithmeticDivision.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

QNEBlock* ArithmeticDivisionFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ArithmeticDivision();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "A" ), QLatin1String( SLOT( setValueA( double ) ) ) );
  b->addPort( QStringLiteral( "/" ), QLatin1String(), false, QNEPort::NoBullet );
  b->addInputPort( QStringLiteral( "B" ), QLatin1String( SLOT( setValueB( double ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( numberChanged( double ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}

void ArithmeticDivision::emitConfigSignals() {
  Q_EMIT numberChanged( result );
}

void ArithmeticDivision::setValueA( double number ) {
  numberA = number;
  operation();
}

void ArithmeticDivision::setValueB( double number ) {
  numberB = number;
  operation();
}

void ArithmeticDivision::operation() {
  if( !qIsNull( numberB ) ) {
    result = numberA / numberB;
  }

  Q_EMIT numberChanged( result );
}
