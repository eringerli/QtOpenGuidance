// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QVector3D>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

#include "kinematic/Plan.h"

// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

class XteGuidance : public BlockBase {
  Q_OBJECT

public:
  explicit XteGuidance( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

public Q_SLOTS:
  void setPose( POSE_SIGNATURE_SLOT );

  void setPlan( const Plan& plan );

  void emitConfigSignals() override;

Q_SIGNALS:
  void xteChanged( NUMBER_SIGNATURE_SIGNAL );
  void headingOfPathChanged( NUMBER_SIGNATURE_SIGNAL );
  void headingDifferenceChanged( NUMBER_SIGNATURE_SIGNAL );
  void curvatureOfPathChanged( NUMBER_SIGNATURE_SIGNAL );
  void passNumberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  Plan                         plan;
  Plan::ConstPrimitiveIterator lastFoundPrimitive;
};

class XteGuidanceFactory : public BlockFactory {
  Q_OBJECT

public:
  XteGuidanceFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "Cross Track Error" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Guidance" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
