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

#include "PoseSimulation.h"

#include "qneblock.h"
#include "qneport.h"

#include <QEvent>
#include <QTime>
#include <QtMath>

#include <QFile>
#include <QFileDialog>
#include <QJsonObject>

#include <QBasicTimer>
#include <QElapsedTimer>
#include <QEvent>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>

#include <QDebug>

#include "helpers/GeoJsonHelper.h"
#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

// http://correll.cs.colorado.edu/?p=1869
// https://github.com/correll/Introduction-to-Autonomous-Robots/releases

//#include <CGAL/Constrained_triangulation_plus_2.h>

//#include <CGAL/Triangulation_vertex_base_with_info_2.h>
//#include <CGAL/Triangulation_face_base_with_info_2.h>

#include <CGAL/boost/graph/graph_traits_Delaunay_triangulation_2.h>
//#include <CGAL/boost/graph/graph_traits_Triangulation_data_structure_2.h>
//#include <CGAL/boost/graph/graph_traits_Constrained_triangulation_plus_2.h>
//#include <CGAL/boost/graph/graph_traits_Constrained_Delaunay_triangulation_2.h>

PoseSimulation::PoseSimulation( QWidget* mainWindow, GeographicConvertionWrapper* tmw ) : mainWindow( mainWindow ), tmw( tmw ) {
  state.setZero();

  setSimulation( false );

  noiseGenerator.seed( std::chrono::system_clock::now().time_since_epoch().count() );

  frontLeftTire  = std::make_unique< VehicleDynamics::TireLinear >( 2400 * 180 / M_PI );
  frontRightTire = std::make_unique< VehicleDynamics::TireLinear >( 2400 * 180 / M_PI );
  rearLeftTire   = std::make_unique< VehicleDynamics::TireLinear >( 3500 * 180 / M_PI );
  rearRightTire  = std::make_unique< VehicleDynamics::TireLinear >( 3500 * 180 / M_PI );

  vehicleDynamics = std::make_unique< VehicleDynamics::VehicleNonLinear3DOF >(
    frontLeftTire.get(),
    /*frontRightTire.get(),*/ rearLeftTire.get() /*, rearRightTire.get()*/ );
}

void
PoseSimulation::timerEvent( QTimerEvent* event ) {
  if( event->timerId() == m_timer.timerId() ) {
    constexpr double msPerS     = 1000;
    double           deltaT     = double( m_time.restart() ) / msPerS;
    double           steerAngle = 0;

    QElapsedTimer timer;
    timer.start();

    if( m_autosteerEnabled ) {
      steerAngle = m_steerAngleFromAutosteer;
    } else {
      steerAngle = m_steerAngle;
    }

    Q_EMIT steeringAngleChanged( steerAngle, CalculationOption::Option::None );
    Q_EMIT velocityChanged( m_velocity, CalculationOption::Option::None );

    auto rollPitchYaw =
      ( lastFoundFaceOrientation * taitBryanToQuaternion( 0, 0, state( int( ThreeWheeledFRHRL::StateNames::Yaw ) ) ) ).conjugate() *
      taitBryanToQuaternion( 0, 0, state( int( ThreeWheeledFRHRL::StateNames::Yaw ) ) );

    //    auto taitBryanAngles = quaternionToTaitBryan( rollPitchYaw );

    //    state( int( ThreeWheeledFRHRL::StateNames::Roll ) ) = taitBryanAngles.y();
    //    state( int( ThreeWheeledFRHRL::StateNames::Pitch ) ) = taitBryanAngles.x();
    //    {
    //      std::cout << "taitBryanAngles: "<<printQuaternionAsTaitBryanDegrees(
    //      rollPitchYaw )<< std::endl;
    //    }

    {
      auto& V      = m_velocity;
      auto  deltaF = degreesToRadians( steerAngle );

      /*if(V>1 || V<-1)*/ {
        constexpr double MinDeltaT = 0.0001;
        double           factor    = std::floor( deltaT / MinDeltaT );
        double           deltaT2   = deltaT / factor;

        //        std::cout << "factor, deltaT, deltaT2" << factor << deltaT << deltaT2 <<
        //        std::endl;

        for( int i = 0; i < factor; ++i ) {
          // constexpr double K = 300000;
          // double force = K *
          // (V-vehicleDynamics->state(long(VehicleDynamics::VehicleNonLinear3DOF::StateNames::V)));

          // vehicleDynamics->step(deltaT2, deltaF, force, force, force, force);

          StateType< double > prediction;
          //          qDebug() << "PoseSimulation::timerEvent" << Caf << Car << Cah;
          simulatorModel.predict( state, deltaT2, V, deltaF, a, b, c, Caf, Car, Cah, m, Iz, sigmaF, sigmaR, sigmaH, Cx, slipX, prediction );
          state = prediction;
        }
      }
    }

    {
      if( tin ) {
        auto point = Point_3( state( int( ThreeWheeledFRHRL::StateNames::X ) ),
                              state( int( ThreeWheeledFRHRL::StateNames::Y ) ),
                              state( int( ThreeWheeledFRHRL::StateNames::Z ) ) );
        auto foundFace = tin->locate( point, lastFoundFace );
        /*if(lastFoundFace != foundFace) */ {
          if( !tin->is_infinite( foundFace ) ) {
            Eigen::Vector3d points[] = { toEigenVector( foundFace->vertex( 0 )->point() ),
                                         toEigenVector( foundFace->vertex( 1 )->point() ),
                                         toEigenVector( foundFace->vertex( 2 )->point() ) };

            auto plane = Plane_3( foundFace->vertex( 0 )->point(), foundFace->vertex( 1 )->point(), foundFace->vertex( 2 )->point() );
            auto line  = Line_3( point, Vector_3( 0, 0, 1 ) );

            auto result = CGAL::intersection( plane, line );

            if( result ) {
              if( auto* const projectedPoint = boost::get< Point_3 >( &*result ) ) {
                state( int( ThreeWheeledFRHRL::StateNames::Z ) ) = projectedPoint->z() /*+ 2*/;
              }
            }

            // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/geometry-of-a-triangle
            auto normalVector        = ( points[1] - points[0] ).cross( points[2] - points[0] );
            lastFoundFaceOrientation = Eigen::Quaterniond::FromTwoVectors( Eigen::Vector3d::UnitZ(), normalVector );

            //            {
            //              std::cout << "lastFoundFaceOrientation:" << printQuaternionAsTaitBryanDegrees(
            //              lastFoundFaceOrientation )
            //                        << ", normalVector:" << printVector( normalVector.normalized() ) << std::endl;
            //            }

          } else {
            //            std::cout << "tin->is_infinite( foundFace )" << std::endl;
            lastFoundFaceOrientation = taitBryanToQuaternion( 0, 0, 0 );
          }

          lastFoundFace = foundFace;
        }
      } else {
        auto file = QFile( "/home/christian/Schreibtisch/QtOpenGuidance/terrain/test4.geojson" );
        file.open( QIODevice::ReadOnly );
        openTINFromFile( file );
      }
    }

    // orientation
    {
      if( noiseOrientationActivated ) {
        m_orientation = taitBryanToQuaternion( state( int( ThreeWheeledFRHRL::StateNames::Yaw ) ) + noiseOrientation( noiseGenerator ),
                                               state( int( ThreeWheeledFRHRL::StateNames::Pitch ) ) + noiseOrientation( noiseGenerator ),
                                               state( int( ThreeWheeledFRHRL::StateNames::Roll ) ) + noiseOrientation( noiseGenerator ) );
      } else {
        m_orientation = taitBryanToQuaternion( state( int( ThreeWheeledFRHRL::StateNames::Yaw ) ),
                                               state( int( ThreeWheeledFRHRL::StateNames::Pitch ) ),
                                               state( int( ThreeWheeledFRHRL::StateNames::Roll ) ) );
      }

      auto orientation = lastFoundFaceOrientation * m_orientation;

      //        std::cout << "lastFoundFaceOrientation: " <<
      //        printQuaternionAsTaitBryanDegrees( lastFoundFaceOrientation )
      //                  << ", m_orientation: " <<
      //                  printQuaternionAsTaitBryanDegrees(m_orientation) << std::endl;

      Q_EMIT orientationChanged( orientation );
      m_orientation = orientation;
    }

    {
      /*static uint8_t counter = 0;

      if( ++counter >= 10 )*/
      {
        //        counter = 0;

        if( noisePositionXYActivated || noisePositionZActivated ) {
          antennaKinematic.setPose( Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::X ) ) + noisePositionXY( noiseGenerator ),
                                                     state( int( ThreeWheeledFRHRL::StateNames::Y ) ) + noisePositionXY( noiseGenerator ),
                                                     state( int( ThreeWheeledFRHRL::StateNames::Z ) ) + noisePositionZ( noiseGenerator ) ),
                                    m_orientation,
                                    CalculationOption::Option::None );
        } else {
          antennaKinematic.setPose( Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::X ) ),
                                                     state( int( ThreeWheeledFRHRL::StateNames::Y ) ),
                                                     state( int( ThreeWheeledFRHRL::StateNames::Z ) ) ),
                                    m_orientation,
                                    CalculationOption::Option::None );
        }

        Q_EMIT velocity3DChanged( Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Vx ) ),
                                                   state( int( ThreeWheeledFRHRL::StateNames::Vy ) ),
                                                   state( int( ThreeWheeledFRHRL::StateNames::Vz ) ) ) );

        // emit signal with antenna offset
        Q_EMIT positionChanged( antennaKinematic.positionCalculated );

        // in global coordinates: WGS84
        {
          double latitude, longitude, height;
          tmw->Reverse( antennaKinematic.positionCalculated.x(),
                        antennaKinematic.positionCalculated.y(),
                        antennaKinematic.positionCalculated.z(),
                        latitude,
                        longitude,
                        height );

          Q_EMIT globalPositionChanged( Eigen::Vector3d( latitude, longitude, height ) );
        }
      }
    }

    Eigen::Vector3d accelerometerData;

    if( noiseAccelerometerActivated ) {
      accelerometerData = Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Ax ) ) + noiseAccelerometer( noiseGenerator ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Ay ) ) + noiseAccelerometer( noiseGenerator ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Az ) ) + noiseAccelerometer( noiseGenerator ) );
    } else {
      accelerometerData = Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Ax ) ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Ay ) ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Az ) ) );
    }

    Eigen::Vector3d gyroData;

    if( noiseGyroActivated ) {
      gyroData = Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Vroll ) ) + noiseGyro( noiseGenerator ),
                                  state( int( ThreeWheeledFRHRL::StateNames::Vpitch ) ) + noiseGyro( noiseGenerator ),
                                  state( int( ThreeWheeledFRHRL::StateNames::Vyaw ) ) + noiseGyro( noiseGenerator ) );
    } else {
      gyroData = Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Vroll ) ),
                                  state( int( ThreeWheeledFRHRL::StateNames::Vpitch ) ),
                                  state( int( ThreeWheeledFRHRL::StateNames::Vyaw ) ) );
    }

    Q_EMIT imuDataChanged( m_orientation, accelerometerData, gyroData );

    //    std::cout
    //      << "Simulator: Alpha F/R/H: "
    //      << radiansToDegrees( state( int( ThreeWheeledFRHRL::StateNames::AlphaFront ) )
    //      )
    //      << ", "
    //      << radiansToDegrees( state( int( ThreeWheeledFRHRL::StateNames::AlphaRear ) )
    //      )
    //      << ", "
    //      << radiansToDegrees( state( int( ThreeWheeledFRHRL::StateNames::AlphaHitch ) )
    //      )
    //      << std::endl;

    double processingTime = double( timer.nsecsElapsed() ) / 1.e6;

    Q_EMIT processingTimeChanged( processingTime, CalculationOption::Option::None );
    Q_EMIT maxProcessingTimeChanged( double( m_interval ), CalculationOption::Option::None );

    //    qDebug() << "Cycle Time PoseSimulation:" << Qt::fixed << qSetRealNumberPrecision( 4 ) << qSetFieldWidth( 7 ) <<
    //    processingTime
    //             << "ms";
  }
}

void
PoseSimulation::emitConfigSignals() {
  Q_EMIT simulatorValuesChanged( a,
                                 b,
                                 c,
                                 // N/rad -> N/°
                                 Caf * M_PI / 180,
                                 Car * M_PI / 180,
                                 Cah * M_PI / 180,
                                 m,
                                 Iz,
                                 sigmaF,
                                 sigmaR,
                                 sigmaH,
                                 Cx,
                                 slipX );

  Q_EMIT steerAngleChanged( m_steerAngle, CalculationOption::Option::None );
  Q_EMIT positionChanged( Eigen::Vector3d( x, y, height ) );
  Q_EMIT orientationChanged( m_orientation );
  Q_EMIT steerAngleChanged( 0, CalculationOption::Option::None );
  Q_EMIT velocityChanged( 0, CalculationOption::Option::None );
}

QJsonObject
PoseSimulation::toJSON() {
  QJsonObject valuesObject;

  valuesObject[QStringLiteral( "a" )]      = this->a;
  valuesObject[QStringLiteral( "b" )]      = this->b;
  valuesObject[QStringLiteral( "c" )]      = this->c;
  valuesObject[QStringLiteral( "Caf" )]    = this->Caf;
  valuesObject[QStringLiteral( "Car" )]    = this->Car;
  valuesObject[QStringLiteral( "Cah" )]    = this->Cah;
  valuesObject[QStringLiteral( "m" )]      = this->m;
  valuesObject[QStringLiteral( "Iz" )]     = this->Iz;
  valuesObject[QStringLiteral( "sigmaF" )] = this->sigmaF;
  valuesObject[QStringLiteral( "sigmaR" )] = this->sigmaR;
  valuesObject[QStringLiteral( "sigmaH" )] = this->sigmaH;
  valuesObject[QStringLiteral( "Cx" )]     = this->Cx;
  valuesObject[QStringLiteral( "slip" )]   = this->slipX;

  return valuesObject;
}

void
PoseSimulation::fromJSON( QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "a" )].isDouble() ) {
    this->a = valuesObject[QStringLiteral( "a" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "b" )].isDouble() ) {
    this->b = valuesObject[QStringLiteral( "b" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "c" )].isDouble() ) {
    this->c = valuesObject[QStringLiteral( "c" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "Caf" )].isDouble() ) {
    this->Caf = valuesObject[QStringLiteral( "Caf" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "Car" )].isDouble() ) {
    this->Car = valuesObject[QStringLiteral( "Car" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "Cah" )].isDouble() ) {
    this->Cah = valuesObject[QStringLiteral( "Cah" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "m" )].isDouble() ) {
    this->m = valuesObject[QStringLiteral( "m" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "Iz" )].isDouble() ) {
    this->Iz = valuesObject[QStringLiteral( "Iz" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "sigmaF" )].isDouble() ) {
    this->sigmaF = valuesObject[QStringLiteral( "sigmaF" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "sigmaR" )].isDouble() ) {
    this->sigmaR = valuesObject[QStringLiteral( "sigmaR" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "sigmaH" )].isDouble() ) {
    this->sigmaH = valuesObject[QStringLiteral( "sigmaH" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "Cx" )].isDouble() ) {
    this->Cx = valuesObject[QStringLiteral( "Cx" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "slip" )].isDouble() ) {
    this->slipX = valuesObject[QStringLiteral( "slip" )].toDouble();
  }
}

void
PoseSimulation::setInterval( int interval ) {
  m_interval = interval;

  setSimulation( m_enabled );

  Q_EMIT intervalChanged( m_interval );
  Q_EMIT maxProcessingTimeChanged( double( m_interval ), CalculationOption::Option::None );
}

void
PoseSimulation::setSimulation( bool enabled ) {
  m_enabled = enabled;

  if( enabled ) {
    m_timer.start( m_interval, Qt::PreciseTimer, this );
    m_time.start();
  } else {
    m_timer.stop();
  }

  Q_EMIT simulationChanged( m_enabled );
}

void
PoseSimulation::setFrequency( double frequency, const CalculationOption::Options ) {
  setInterval( 1000 / frequency );
}

void
PoseSimulation::setSteerAngle( double steerAngle, const CalculationOption::Options ) {
  m_steerAngle = steerAngle;
}

void
PoseSimulation::setVelocity( double velocity, const CalculationOption::Options ) {
  m_velocity = velocity;
}

void
PoseSimulation::setSteerAngleFromAutosteer( double steerAngle, const CalculationOption::Options ) {
  m_steerAngleFromAutosteer = steerAngle;
}

void
PoseSimulation::setInitialWGS84Position( const Eigen::Vector3d& position ) {
  tmw->Reset( position.x(), position.y(), position.z() );
}

void
PoseSimulation::autosteerEnabled( bool enabled ) {
  m_autosteerEnabled = enabled;
}

void
PoseSimulation::setSimulatorValues( const double a,
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
                                    const double slipX ) {
  this->a = a;
  this->b = b;
  this->c = c;
  // N/° -> N/rad
  this->Caf    = Caf * 180 / M_PI;
  this->Car    = Car * 180 / M_PI;
  this->Cah    = Cah * 180 / M_PI;
  this->m      = m;
  this->Iz     = Iz;
  this->sigmaF = sigmaF;
  this->sigmaR = sigmaR;
  this->sigmaH = sigmaH;
  this->Cx     = Cx;
  this->slipX  = slipX;
}

void
PoseSimulation::setNoiseStandartDeviations(
  double noisePositionXY, double noisePositionZ, double noiseOrientation, double noiseAccelerometer, double noiseGyro ) {
  if( !qIsNull( noisePositionXY ) ) {
    noisePositionXYActivated = true;
    this->noisePositionXY    = std::normal_distribution< double >( 0, noisePositionXY );
  } else {
    noisePositionXYActivated = false;
  }

  if( !qIsNull( noisePositionZ ) ) {
    noisePositionZActivated = true;
    this->noisePositionZ    = std::normal_distribution< double >( 0, noisePositionZ );
  } else {
    noisePositionZActivated = false;
  }

  if( !qIsNull( noiseOrientation ) ) {
    noiseOrientationActivated = true;
    this->noiseOrientation    = std::normal_distribution< double >( 0, noiseOrientation );
  } else {
    noiseOrientationActivated = false;
  }

  if( !qIsNull( noiseAccelerometer ) ) {
    noiseAccelerometerActivated = true;
    this->noiseAccelerometer    = std::normal_distribution< double >( 0, noiseAccelerometer );
  } else {
    noiseAccelerometerActivated = false;
  }

  if( !qIsNull( noiseGyro ) ) {
    noiseGyroActivated = true;
    this->noiseGyro    = std::normal_distribution< double >( 0, noiseGyro );
  } else {
    noiseGyroActivated = false;
  }
}

void
PoseSimulation::openTIN() {
  QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
  QString dir;

  auto* fileDialog = new QFileDialog( mainWindow, tr( "Open Saved Field" ), dir, selectedFilter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  fileDialog->setNameFilter( tr( "All Files (*);;GeoJSON Files (*.geojson)" ) );

  // connect the signal QFileDialog::urlSelected to a lambda, which opens the file.
  // this is needed, as the file dialog on android is asynchonous, so you have to connect
  // to the signals instead of using the static functions for the dialogs
#ifdef Q_OS_ANDROID
  QObject::connect( fileDialog, &QFileDialog::urlSelected, this, [this, fileDialog]( QUrl fileName ) {
    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile( QUrl::fromPercentEncoding( fileName.toString().split( QStringLiteral( "%3A" ) ).at( 1 ).toUtf8() ) );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {
        openFieldFromFile( loadFile );
      }
    }

    // block all further signals, so no double opening happens
    fileDialog->blockSignals( true );

    fileDialog->deleteLater();
  } );
#else
  QObject::connect( fileDialog, &QFileDialog::fileSelected, mainWindow, [this, fileDialog]( const QString& fileName ) {
    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile( fileName );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {
        openTINFromFile( loadFile );
      }
    }

    // block all further signals, so no double opening happens
    fileDialog->blockSignals( true );

    fileDialog->deleteLater();
  } );
#endif

  // connect finished to deleteLater, so the dialog gets deleted when Cancel is pressed
  QObject::connect( fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater );

  fileDialog->open();
}

void
PoseSimulation::openTINFromFile( QFile& file ) {
  auto geoJsonHelper = GeoJsonHelper( file );

  std::vector< Point_3 > points;

  for( const auto& member : geoJsonHelper.members ) {
    switch( member.first ) {
      case GeoJsonHelper::GeometryType::MultiPoint: {
        for( const auto& point : std::get< GeoJsonHelper::MultiPointType >( member.second ) ) {
          auto tmwPoint = tmw->Forward( point );
          points.emplace_back( toPoint3( tmwPoint ) );
        }
      } break;

      default:
        break;
    }
  }

  tin = make_unique< DelaunayTriangulationProjectedXY >( points.cbegin(), points.cend() );

  auto surfaceMesh = make_shared< SurfaceMesh_3 >();
  CGAL::copy_face_graph( *tin, *surfaceMesh );

  Q_EMIT surfaceChanged( surfaceMesh );
}

void
PoseSimulation::setWheelbase( double wheelbase, const CalculationOption::Options ) {
  if( !qFuzzyIsNull( wheelbase ) ) {
    m_wheelbase = wheelbase;
  }
}

void
PoseSimulation::setAntennaOffset( const Eigen::Vector3d& offset ) {
  auto offsetTmp = offset;
  offsetTmp.x() -= b;
  antennaKinematic.setOffset( -offsetTmp );
}

QNEBlock*
PoseSimulationFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new PoseSimulation( mainWindow, geographicConvertionWrapper );
  auto* b   = createBaseBlock( scene, obj, id, true );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Antenna Position" ), QLatin1String( SLOT( setAntennaOffset( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Initial WGS84 Position" ), QLatin1String( SLOT( setInitialWGS84Position( const Eigen::Vector3d& ) ) ) );

  b->addOutputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SIGNAL( globalPositionChanged( const Eigen::Vector3d& ) ) ) );
  b->addOutputPort( QStringLiteral( "Velocity 3D" ), QLatin1String( SIGNAL( velocity3DChanged( const Eigen::Vector3d& ) ) ) );

  b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( positionChanged( const Eigen::Vector3d& ) ) ) );
  b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( const Eigen::Quaterniond& ) ) ) );
  b->addOutputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SIGNAL( steeringAngleChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Velocity" ), QLatin1String( SIGNAL( velocityChanged( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort(
    QStringLiteral( "IMU Data" ),
    QLatin1String( SIGNAL( imuDataChanged( const Eigen::Quaterniond&, const Eigen::Vector3d&, const Eigen::Vector3d& ) ) ) );

  b->addOutputPort( QStringLiteral( "Processing Time [ms]" ), QLatin1String( SIGNAL( processingTimeChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Max Processing Time [ms]" ), QLatin1String( SIGNAL( maxProcessingTimeChanged( NUMBER_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "Autosteer Enabled" ), QLatin1String( SLOT( autosteerEnabled( ACTION_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Autosteer Steering Angle" ), QLatin1String( SLOT( setSteerAngleFromAutosteer( NUMBER_SIGNATURE ) ) ) );

  return b;
}

#include "moc_PoseSimulation.cpp"
