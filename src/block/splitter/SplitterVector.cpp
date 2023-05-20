// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SplitterVector.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
SplitterVector::emitConfigSignals() {
  Q_EMIT numberChangedX( 0, CalculationOption::Option::None );
  Q_EMIT numberChangedY( 0, CalculationOption::Option::None );
  Q_EMIT numberChangedZ( 0, CalculationOption::Option::None );
}

void
SplitterVector::setVector( const Eigen::Vector3d& vector, const CalculationOption::Options ) {
  Q_EMIT numberChangedX( vector.x(), CalculationOption::Option::None );
  Q_EMIT numberChangedY( vector.y(), CalculationOption::Option::None );
  Q_EMIT numberChangedZ( vector.z(), CalculationOption::Option::None );
}

QNEBlock*
SplitterVectorFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new SplitterVector();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Vector" ), QLatin1String( SLOT( setVector( VECTOR_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "X" ), QLatin1String( SIGNAL( numberChangedX( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Y" ), QLatin1String( SIGNAL( numberChangedY( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Z" ), QLatin1String( SIGNAL( numberChangedZ( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  b->setBrush( converterColor );

  return b;
}
