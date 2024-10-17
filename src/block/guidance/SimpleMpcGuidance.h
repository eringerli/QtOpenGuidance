// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>

#include <QObject>
#include <QVector3D>

#include "block/BlockBase.h"
#include "block/kinematic/FixedKinematicPrimitive.h"

#include "helpers/anglesHelper.h"
#include "helpers/eigenHelper.h"

#include "kddockwidgets/KDDockWidgets.h"

#include "kinematic/Plan.h"

#include "3d/qt3dForwards.h"

class MyMainWindow;
class QMenu;

// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

// Q_DECLARE_METATYPE( std::vector< double > )
// Q_DECLARE_METATYPE( std::shared_ptr< std::vector< double > > )

class SimpleMpcGuidance : public BlockBase {
  Q_OBJECT

public:
  explicit SimpleMpcGuidance( const BlockBaseId idHint, const bool systemBlock, const QString type, const BlockBase::TypeColor typeColor )
      : BlockBase( idHint, systemBlock, type, typeColor ) {}

public Q_SLOTS:
  void setWheelbase( NUMBER_SIGNATURE_SLOT );
  void setTrackwidth( NUMBER_SIGNATURE_SLOT );
  void setAntennaPosition( VECTOR_SIGNATURE_SLOT );

  void setWindow( NUMBER_SIGNATURE_SLOT );
  void setSteps( NUMBER_SIGNATURE_SLOT );
  void setSteerAngleThreshold( NUMBER_SIGNATURE_SLOT );
  void setWeight( NUMBER_SIGNATURE_SLOT );
  void setMaxSlewRateSteering( NUMBER_SIGNATURE_SLOT );

  void setPlan( const Plan& plan );

  void setSteerAngle( NUMBER_SIGNATURE_SLOT );
  void setVelocity( NUMBER_SIGNATURE_SLOT );
  void setPose( POSE_SIGNATURE_SLOT );

  void setXTE( NUMBER_SIGNATURE_SLOT );
  void setHeadingOfPath( NUMBER_SIGNATURE_SLOT );
  void setPoseResult( POSE_SIGNATURE_SLOT );

  void emitConfigSignals() override;

Q_SIGNALS:
  void triggerPose( POSE_SIGNATURE_SIGNAL );
  void steerAngleChanged( NUMBER_SIGNATURE_SIGNAL );

  void dockValuesChanged( std::shared_ptr< std::vector< double > >, std::shared_ptr< std::vector< double > > );
  void bufferChanged( const QVector< QVector3D >& pos );
  void buffer2Changed( const QVector< QVector3D >& pos );

private:
  double wheelbase  = 2.4;
  double trackwidth = 2.;

  double window                 = 4.;
  double steps                  = 40;
  double steerAngleThresholdRad = degreesToRadians( 0.5 );

  double steerAngleRadians = 0;
  double velocity          = 0;

  double weight                       = 1;
  double maxSlewRateSteeringRadPerSec = degreesToRadians( 32. );

  Plan                         plan;
  Plan::ConstPrimitiveIterator lastFoundPrimitive;

  std::shared_ptr< std::vector< double > > recordXteTo           = nullptr;
  std::shared_ptr< std::vector< double > > recordHeadingTo       = nullptr;
  std::shared_ptr< std::vector< double > > recordHeadingOfPathTo = nullptr;

  std::shared_ptr< std::vector< double > >          recordXteForPointsTo = nullptr;
  std::shared_ptr< std::vector< Eigen::Vector3d > > recordPointsTo       = nullptr;

  FixedKinematicPrimitive antennaKinematic = FixedKinematicPrimitive( 0, false, "FixedKinematicPrimitive", BlockBase::TypeColor::System );

  double runMpc( const double           dT,
                 const double           steeringAngleToCalculateWithRadians,
                 const double           yawToStartFromRadians,
                 const Eigen::Vector3d& positionToStartFrom );

  void goldenRatioMpc( const Eigen::Vector3d&                    positionPose,
                       const Eigen::Quaterniond&                 orientationPose,
                       const double                              dT,
                       const double                              yawToStartFromRadians,
                       std::shared_ptr< std::vector< double > >& steeringAngles,
                       std::shared_ptr< std::vector< double > >& xtesCostFunctionResults );

  bool   newtonMethodMpc( const Eigen::Vector3d&                    positionPose,
                          const Eigen::Quaterniond&                 orientationPose,
                          const double                              dT,
                          const double                              yawToStartFromRadians,
                          const double                              steeringAngleToCalculateWithRadians,
                          std::shared_ptr< std::vector< double > >& steeringAngles,
                          std::shared_ptr< std::vector< double > >& xtesCostFunctionResults );
  double calculateWeightFactor( const double index, const double size );
};

class SimpleMpcGuidanceFactory : public BlockFactory {
  Q_OBJECT

public:
  SimpleMpcGuidanceFactory(
    QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu, Qt3DCore::QEntity* rootEntity )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ), rootEntity( rootEntity ) {
    typeColor = BlockBase::TypeColor::Arithmetic;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Simple Model Predictive Control" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Guidance" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu       = nullptr;
  Qt3DCore::QEntity*      rootEntity = nullptr;
};
