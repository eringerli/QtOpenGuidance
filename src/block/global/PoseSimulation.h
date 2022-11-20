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
#include <random>

#include <QBasicTimer>
#include <QElapsedTimer>
#include <QObject>

#include "3d/qt3dForwards.h"

#include "block/BlockBase.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

#include "block/kinematic/FixedKinematicPrimitive.h"
#include "helpers/GeographicConvertionWrapper.h"

#include "kinematic/VehicleDynamics/TireLinear.h"
#include "kinematic/VehicleDynamics/VehicleNonLinear3DOF.h"
#include "kinematic/VehicleDynamics/VehicleNonLinear4DOF.h"

#include "filter/3wFRHRL/SystemModel.h"

using namespace std;
using namespace GeographicLib;

#include <CGAL/Point_set_3.h>
using PointSet_3 = CGAL::Point_set_3< Point_3 >;

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Projection_traits_xy_3.h>
using ProjectionTraitsXY               = CGAL::Projection_traits_xy_3< Epick >;
using DelaunayTriangulationProjectedXY = CGAL::Delaunay_triangulation_2< ProjectionTraitsXY >;

#include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Surface_mesh.h>
using SurfaceMesh_3 = CGAL::Surface_mesh< Point_3 >;

class QFile;

class PoseSimulation : public BlockBase {
  Q_OBJECT

public:
  explicit PoseSimulation( QWidget* mainWindow, GeographicConvertionWrapper* tmw );

public Q_SLOTS:
  void setInterval( const int interval );
  void setSimulation( ACTION_SIGNATURE );
  void setFrequency( NUMBER_SIGNATURE_SLOT );
  void setSteerAngle( NUMBER_SIGNATURE_SLOT );
  void setVelocity( NUMBER_SIGNATURE_SLOT );
  void setSteerAngleFromAutosteer( NUMBER_SIGNATURE_SLOT );

  void setWheelbase( NUMBER_SIGNATURE_SLOT );
  void setAntennaOffset( const Eigen::Vector3d& offset );
  void setInitialWGS84Position( const Eigen::Vector3d& position );
  void autosteerEnabled( ACTION_SIGNATURE );

  void setSimulatorValues( const double a,
                           const double b,
                           const double c,
                           const double Caf,
                           const double Car,
                           const double Cah,
                           const double m,
                           const double Iz,
                           const double sigmaF,
                           const double sigmaR,
                           const double sigmaH,
                           const double Cx,
                           const double slipX );

  void setNoiseStandartDeviations( const double noisePositionXY,
                                   const double noisePositionZ,
                                   const double noiseOrientation,
                                   const double noiseAccelerometer,
                                   const double noiseGyro );

  void openTIN();
  void openTINFromString( QString fileName );
  void openTINFromFile( QFile& file );

Q_SIGNALS:
  void simulatorValuesChanged( const double a,
                               const double b,
                               const double c,
                               const double Caf,
                               const double Car,
                               const double Cah,
                               const double m,
                               const double Iz,
                               const double sigmaF,
                               const double sigmaR,
                               const double sigmaH,
                               const double Cx,
                               const double slip );

  void simulationChanged( const bool );
  void intervalChanged( const int );

  void steerAngleChanged( NUMBER_SIGNATURE_SIGNAL );

  void antennaPositionChanged( const Eigen::Vector3d& );

  void steeringAngleChanged( NUMBER_SIGNATURE_SIGNAL );
  void positionChanged( const Eigen::Vector3d& );
  void globalPositionChanged( const Eigen::Vector3d& );
  void velocity3DChanged( const Eigen::Vector3d& );
  void orientationChanged( const Eigen::Quaterniond& );
  void velocityChanged( NUMBER_SIGNATURE_SIGNAL );

  void imuDataChanged( const Eigen::Quaterniond&, const Eigen::Vector3d&, const Eigen::Vector3d& );

  void maxProcessingTimeChanged( NUMBER_SIGNATURE_SIGNAL );
  void processingTimeChanged( NUMBER_SIGNATURE_SIGNAL );

  void surfaceChanged( std::shared_ptr< SurfaceMesh_3 > );

public:
  virtual void emitConfigSignals() override;

  virtual QJsonObject toJSON() override;

  virtual void fromJSON( QJsonObject& ) override;

protected:
  void timerEvent( QTimerEvent* event ) override;

private:
  QWidget* mainWindow = nullptr;

  bool m_enabled          = false;
  bool m_autosteerEnabled = false;
  int  m_interval         = 50;

  QBasicTimer   m_timer;
  int           m_timerId{};
  QElapsedTimer m_time;

  double m_steerAngle              = 0;
  double m_steerAngleFromAutosteer = 0;
  double m_velocity                = 0;

  double a      = 1;
  double b      = 2;
  double c      = 2.5;
  double Caf    = 2400 * 180 / M_PI;
  double Car    = 5000 * 180 / M_PI;
  double Cah    = 750 * 180 / M_PI;
  double m      = 5500;
  double Iz     = 9500;
  double sigmaF = 0.34;
  double sigmaR = 0.34;
  double sigmaH = 0.40;
  double Cx     = 5000 * 180 / M_PI;
  double slipX  = 0.025;

  double m_wheelbase = 2.4f;

  Eigen::Quaterniond m_orientation = Eigen::Quaterniond( 0, 0, 0, 0 );

  double                       x           = 0;
  double                       y           = 0;
  double                       height      = 0;
  double                       lastHeading = 0;
  GeographicConvertionWrapper* tmw         = nullptr;
  FixedKinematicPrimitive      antennaKinematic;

  std::unique_ptr< VehicleDynamics::VehicleNonLinear3DOF > vehicleDynamics;
  std::unique_ptr< VehicleDynamics::TireLinear >           frontLeftTire;
  std::unique_ptr< VehicleDynamics::TireLinear >           frontRightTire;
  std::unique_ptr< VehicleDynamics::TireLinear >           rearLeftTire;
  std::unique_ptr< VehicleDynamics::TireLinear >           rearRightTire;

  template< typename T_ >
  using StateType = ThreeWheeledFRHRL::State< T_ >;
  StateType< double >                                      state;
  ThreeWheeledFRHRL::TractorImuGpsFusionModel< StateType > simulatorModel;

  std::default_random_engine         noiseGenerator;
  bool                               noisePositionXYActivated    = false;
  std::normal_distribution< double > noisePositionXY             = std::normal_distribution< double >( 0, 0.02 );
  bool                               noisePositionZActivated     = false;
  std::normal_distribution< double > noisePositionZ              = std::normal_distribution< double >( 0, 0.04 );
  bool                               noiseOrientationActivated   = false;
  std::normal_distribution< double > noiseOrientation            = std::normal_distribution< double >( 0, 0.001 );
  bool                               noiseAccelerometerActivated = false;
  std::normal_distribution< double > noiseAccelerometer          = std::normal_distribution< double >( 0, 0.01 );
  bool                               noiseGyroActivated          = false;
  std::normal_distribution< double > noiseGyro                   = std::normal_distribution< double >( 0, 0.01 );

  std::unique_ptr< DelaunayTriangulationProjectedXY > tin;
  DelaunayTriangulationProjectedXY::Face_handle       lastFoundFace;
  Eigen::Quaterniond                                  lastFoundFaceOrientation = taitBryanToQuaternion( 0, 0, 0 );
};

class PoseSimulationFactory : public BlockFactory {
  Q_OBJECT

public:
  PoseSimulationFactory( QThread*                     thread,
                         QWidget*                     mainWindow,
                         GeographicConvertionWrapper* geographicConvertionWrapper )
      : BlockFactory( thread )
      , mainWindow( mainWindow )
      , geographicConvertionWrapper( geographicConvertionWrapper ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Pose Simulation" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Base Blocks" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  QWidget*                     mainWindow                  = nullptr;
  GeographicConvertionWrapper* geographicConvertionWrapper = nullptr;
};
