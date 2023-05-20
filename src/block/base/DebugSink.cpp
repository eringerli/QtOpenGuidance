// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DebugSink.h"

#include <QByteArray>
#include <QDateTime>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

#include <QDebug>

#include "qneblock.h"
#include "qneport.h"

void
DebugSink::setPosition( QVector3D value ) {
  if( block ) {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << value;
  } else {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
  }
}

void
DebugSink::setWGS84Position( double latitude, double longitude, double height ) {
  if( block ) {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << latitude << longitude << height;
  } else {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << latitude << longitude << height;
  }
}

void
DebugSink::setOrientation( const Eigen::Quaterniond& value ) {
  auto taitBryan = quaternionToTaitBryan( value );

  if( block ) {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << taitBryan.y() << taitBryan.x() << taitBryan.z();
  } else {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << taitBryan.y() << taitBryan.x() << taitBryan.z();
  }
}

void
DebugSink::setSteeringAngle( double value, const CalculationOption::Options ) {
  if( block ) {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << value;
  } else {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
  }
}

void
DebugSink::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
  auto taitBryan = quaternionToTaitBryan( orientation );

  if( block ) {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << position.x() << position.y() << position.z() << taitBryan.y()
             << taitBryan.x() << taitBryan.z() << options;
  } else {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << position.x() << position.y() << position.z() << taitBryan.y() << taitBryan.x()
             << taitBryan.z() << options;
  }
}

void
DebugSink::setData( const QByteArray& data ) {
  if( block ) {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << block->getName() << "Data:" << data;
  } else {
    qDebug() << QDateTime::currentMSecsSinceEpoch() << "Data:" << data;
  }
}

QNEBlock*
DebugSinkFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new DebugSink;
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  auto debugSink = qobject_cast< DebugSink* >( obj );

  if( debugSink ) {
    debugSink->block = b;
  }

  b->addInputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SLOT( setWGS84Position( double, double, double ) ) ) );
  b->addInputPort( QStringLiteral( "Position" ), QLatin1String( SLOT( setPosition( VECTOR_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( Eigen::Quaterniond ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ) );

  return b;
}
