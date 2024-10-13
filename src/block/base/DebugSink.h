//// Copyright( C ) Christian Riggenbach
//// SPDX-License-Identifier: GPL-3.0-or-later

// #pragma once

// #include "block/BlockBase.h"

// #include "helpers/eigenHelper.h"

// class DebugSink : public BlockBase {
//   Q_OBJECT

// public:
//   explicit DebugSink( const int idHint,const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

// public Q_SLOTS:
//   void setPosition( QVector3D value );

//  void setWGS84Position( double latitude, double longitude, double height );

//  void setOrientation( const Eigen::Quaterniond& value );

//  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

//  void setPose( POSE_SIGNATURE_SLOT );

//  void setData( const QByteArray& data );
//};

// class DebugSinkFactory : public BlockFactory {
//   Q_OBJECT

// public:
//   DebugSinkFactory( QThread* thread ) : BlockFactory( thread, false ) {}

//  QString getNameOfFactory() const override { return QStringLiteral( "Console Output" ); }

//  QString getCategoryOfFactory() const override { return QStringLiteral( "Base Blocks" ); }

//  virtual std::unique_ptr< BlockBase > createBlock(int idHint = 0) override;
//};
