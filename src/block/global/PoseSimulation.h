// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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

#include "helpers/signatures.h"
#include "kinematic/VehicleDynamics/TireLinear.h"
#include "kinematic/VehicleDynamics/VehicleNonLinear3DOF.h"
#include "kinematic/VehicleDynamics/VehicleNonLinear4DOF.h"

#include "filter/3wFRHRL/SystemModel.h"
#include "filter/SlewRateLimiter.h"

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
class OpenSaveHelper;

namespace Qt3DCore {
  class QEntity;
};

class PoseSimulation : public BlockBase {
  Q_OBJECT

public:
  explicit PoseSimulation(
    QWidget* mainWindow, GeographicConvertionWrapper* tmw, const int idHint, const bool systemBlock, const QString type );

public Q_SLOTS:
  void setInterval( const int interval );
  void setSimulation( ACTION_SIGNATURE_SLOT );
  void setFrequency( NUMBER_SIGNATURE_SLOT );
  void setSteerAngle( NUMBER_SIGNATURE_SLOT );
  void setVelocity( NUMBER_SIGNATURE_SLOT );
  void setSteerAngleFromAutosteer( NUMBER_SIGNATURE_SLOT );

  void setWheelbase( NUMBER_SIGNATURE_SLOT );
  void setAntennaOffset( VECTOR_SIGNATURE_SLOT );
  void setInitialWGS84Position( VECTOR_SIGNATURE_SLOT );
  void autosteerEnabled( ACTION_SIGNATURE_SLOT );

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
                           const double slipX,
                           const double slewRateAutosteerSteering );

  void setNoiseStandartDeviations( const double noisePositionXY,
                                   const double noisePositionZ,
                                   const double noiseOrientation,
                                   const double noiseAccelerometer,
                                   const double noiseGyro );

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
                               const double slip,
                               const double slewRateAutosteerSteering );

  void simulationChanged( const bool );
  void intervalChanged( const int );

  void steerAngleChanged( NUMBER_SIGNATURE_SIGNAL );

  void antennaPositionChanged( VECTOR_SIGNATURE_SIGNAL );

  void steeringAngleChanged( NUMBER_SIGNATURE_SIGNAL );
  void positionChanged( VECTOR_SIGNATURE_SIGNAL );
  void globalPositionChanged( VECTOR_SIGNATURE_SIGNAL );
  void velocity3DChanged( VECTOR_SIGNATURE_SIGNAL );
  void orientationChanged( ORIENTATION_SIGNATURE_SIGNAL );
  void velocityChanged( NUMBER_SIGNATURE_SIGNAL );

  void imuDataChanged( IMU_SIGNATURE_SIGNAL );

  void maxProcessingTimeChanged( NUMBER_SIGNATURE_SIGNAL );
  void processingTimeChanged( NUMBER_SIGNATURE_SIGNAL );

  void surfaceChanged( std::shared_ptr< SurfaceMesh_3 > );

public:
  virtual void emitConfigSignals() override;

  virtual void toJSON( QJsonObject& json ) const override;

  virtual void fromJSON( const QJsonObject& ) override;

protected:
  void timerEvent( QTimerEvent* event ) override;

public:
  OpenSaveHelper* openSaveHelper = nullptr;

private:
  QWidget* mainWindow = nullptr;

  bool m_enabled          = false;
  bool m_autosteerEnabled = false;
  int  m_interval         = 50;

  QBasicTimer   m_timer;
  int           m_timerId = 0;
  QElapsedTimer m_time;

  double                    m_steerAngle                         = 0;
  double                    m_steerAngleFromAutosteer            = 0;
  SlewRateLimiter< double > m_steerAngleAutosteerSlewRateLimiter = SlewRateLimiter( 32. );
  double                    m_velocity                           = 0;

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

  double                       x                = 0;
  double                       y                = 0;
  double                       height           = 0;
  double                       lastHeading      = 0;
  GeographicConvertionWrapper* tmw              = nullptr;
  FixedKinematicPrimitive      antennaKinematic = FixedKinematicPrimitive( 0, false, "FixedKinematicPrimitive" );

  std::unique_ptr< VehicleDynamics::VehicleNonLinear3DOF > vehicleDynamics;
  std::unique_ptr< VehicleDynamics::TireLinear >           frontLeftTire;
  std::unique_ptr< VehicleDynamics::TireLinear >           frontRightTire;
  std::unique_ptr< VehicleDynamics::TireLinear >           rearLeftTire;
  std::unique_ptr< VehicleDynamics::TireLinear >           rearRightTire;

  template< typename T_ >
  using StateType = ThreeWheeledFRHRL::State< T_ >;
  StateType< double >                                      state;
  ThreeWheeledFRHRL::TractorImuGpsFusionModel< StateType > simulatorModel;

  bool noisePositionXYActivated    = false;
  bool noisePositionZActivated     = false;
  bool noiseOrientationActivated   = false;
  bool noiseAccelerometerActivated = false;
  bool noiseGyroActivated          = false;

  std::default_random_engine         noiseGenerator;
  std::normal_distribution< double > noisePositionXY    = std::normal_distribution< double >( 0, 0.02 );
  std::normal_distribution< double > noisePositionZ     = std::normal_distribution< double >( 0, 0.04 );
  std::normal_distribution< double > noiseOrientation   = std::normal_distribution< double >( 0, 0.001 );
  std::normal_distribution< double > noiseAccelerometer = std::normal_distribution< double >( 0, 0.01 );
  std::normal_distribution< double > noiseGyro          = std::normal_distribution< double >( 0, 0.01 );

  std::unique_ptr< DelaunayTriangulationProjectedXY > tin;
  DelaunayTriangulationProjectedXY::Face_handle       lastFoundFace;
  Eigen::Quaterniond                                  lastFoundFaceOrientation = taitBryanToQuaternion( 0, 0, 0 );
};

class PoseSimulationFactory : public BlockFactory {
  Q_OBJECT

public:
  PoseSimulationFactory( QThread*                     thread,
                         QWidget*                     mainWindow,
                         GeographicConvertionWrapper* geographicConvertionWrapper,
                         Qt3DCore::QEntity*           rootEntity,
                         bool                         usePBR )
      : BlockFactory( thread, true )
      , mainWindow( mainWindow )
      , geographicConvertionWrapper( geographicConvertionWrapper )
      , rootEntity( rootEntity )
      , usePBR( usePBR ) {
    typeColor = TypeColor::Model;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Pose Simulation" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Base Blocks" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;

private:
  QWidget*                     mainWindow                  = nullptr;
  GeographicConvertionWrapper* geographicConvertionWrapper = nullptr;
  Qt3DCore::QEntity*           rootEntity                  = nullptr;
  bool                         usePBR                      = false;
};
