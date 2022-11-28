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

#include <memory>

#include <QObject>
#include <QVector3D>

#include "block/BlockBase.h"
#include "block/kinematic/FixedKinematicPrimitive.h"

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
  explicit SimpleMpcGuidance() : BlockBase() {}

public Q_SLOTS:
  void setWheelbase( NUMBER_SIGNATURE_SLOT );
  void setTrackwidth( NUMBER_SIGNATURE_SLOT );
  void setAntennaPosition( const Eigen::Vector3d& offset );

  void setWindow( NUMBER_SIGNATURE_SLOT );
  void setSteps( NUMBER_SIGNATURE_SLOT );
  void setSteerAngleThreshold( NUMBER_SIGNATURE_SLOT );
  void setWeight( NUMBER_SIGNATURE_SLOT );

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

  double steerAngle = 0;
  double velocity   = 0;

  double weight = 1;

  Plan                         plan;
  Plan::ConstPrimitiveIterator lastFoundPrimitive;

  std::shared_ptr< std::vector< double > > recordXteTo           = nullptr;
  std::shared_ptr< std::vector< double > > recordHeadingTo       = nullptr;
  std::shared_ptr< std::vector< double > > recordHeadingOfPathTo = nullptr;

  std::shared_ptr< std::vector< double > >          recordXteForPointsTo = nullptr;
  std::shared_ptr< std::vector< Eigen::Vector3d > > recordPointsTo       = nullptr;

  FixedKinematicPrimitive antennaKinematic;

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

  bool newtonMethodMpc( const Eigen::Vector3d&                    positionPose,
                        const Eigen::Quaterniond&                 orientationPose,
                        const double                              dT,
                        const double                              yawToStartFromRadians,
                        const double                              steeringAngleToCalculateWithRadians,
                        std::shared_ptr< std::vector< double > >& steeringAngles,
                        std::shared_ptr< std::vector< double > >& xtesCostFunctionResults );
};

class SimpleMpcGuidanceFactory : public BlockFactory {
  Q_OBJECT

public:
  SimpleMpcGuidanceFactory(
    QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu, Qt3DCore::QEntity* rootEntity )
      : BlockFactory( thread ), mainWindow( mainWindow ), location( location ), menu( menu ), rootEntity( rootEntity ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Model Predictive Control" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Guidance" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu       = nullptr;
  Qt3DCore::QEntity*      rootEntity = nullptr;
};
