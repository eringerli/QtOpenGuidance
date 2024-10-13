//// Copyright( C ) Christian Riggenbach
//// SPDX-License-Identifier: GPL-3.0-or-later

// #include "DebugSink.h"

// #include <QByteArray>
// #include <QDateTime>

// #include "block/BlockBase.h"

// #include "helpers/eigenHelper.h"

// #include <QDebug>

//
//

// void
// DebugSink::setPosition( QVector3D value ) {
//   if( block ) {
//     qDebug() << QDateTime::currentMSecsSinceEpoch() << block->block->name() << value;
//   } else {
//     qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
//   }
// }

// void
// DebugSink::setWGS84Position( double latitude, double longitude, double height ) {
//   if( block ) {
//     qDebug() << QDateTime::currentMSecsSinceEpoch() << block->block->name() << latitude << longitude << height;
//   } else {
//     qDebug() << QDateTime::currentMSecsSinceEpoch() << latitude << longitude << height;
//   }
// }

// void
// DebugSink::setOrientation( const Eigen::Quaterniond& value ) {
//   auto taitBryan = quaternionToTaitBryan( value );

//  if( block ) {
//    qDebug() << QDateTime::currentMSecsSinceEpoch() << block->block->name() << taitBryan.y() << taitBryan.x() << taitBryan.z();
//  } else {
//    qDebug() << QDateTime::currentMSecsSinceEpoch() << taitBryan.y() << taitBryan.x() << taitBryan.z();
//  }
//}

// void
// DebugSink::setSteeringAngle( double value, const CalculationOption::Options ) {
//   if( block ) {
//     qDebug() << QDateTime::currentMSecsSinceEpoch() << block->block->name() << value;
//   } else {
//     qDebug() << QDateTime::currentMSecsSinceEpoch() << value;
//   }
// }

// void
// DebugSink::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
//   auto taitBryan = quaternionToTaitBryan( orientation );

//  if( block ) {
//    qDebug() << QDateTime::currentMSecsSinceEpoch() << block->block->name() << position.x() << position.y() << position.z() <<
//    taitBryan.y()
//             << taitBryan.x() << taitBryan.z() << options;
//  } else {
//    qDebug() << QDateTime::currentMSecsSinceEpoch() << position.x() << position.y() << position.z() << taitBryan.y() << taitBryan.x()
//             << taitBryan.z() << options;
//  }
//}

// void
// DebugSink::setData( const QByteArray& data ) {
//   if( block ) {
//     qDebug() << QDateTime::currentMSecsSinceEpoch() << block->block->name() << "Data:" << data;
//   } else {
//     qDebug() << QDateTime::currentMSecsSinceEpoch() << "Data:" << data;
//   }
// }

// QNEBlock*
// DebugSinkFactory::createBlock() {
//   auto* obj = new DebugSink;
//
//

//  auto debugSink = qobject_cast< DebugSink* >( obj );

//  if( debugSink ) {
//    debugSink->block = b;
//  }

//  obj->addInputPort( QStringLiteral( "WGS84 Position" ), obj.get(), QLatin1StringView( SLOT( setWGS84Position( double, double, double ) )
//  ) ); obj->addInputPort(QStringLiteral("Position"),obj,QLatin1StringView( SLOT( setPosition( VECTOR_SIGNATURE ) ) ) );
//  obj->addInputPort(QStringLiteral("Orientation"),obj,QLatin1StringView( SLOT( setOrientation( Eigen::Quaterniond ) ) ) );
//  obj->addInputPort(QStringLiteral("Pose"),obj,QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
//  obj->addInputPort( QStringLiteral( "Steering Angle" ), obj.get(), QLatin1StringView( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );
//  obj->addInputPort(QStringLiteral("Data"),obj,QLatin1StringView( SLOT( setData( const QByteArray& ) ) ) );

//
//}
