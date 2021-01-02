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

#include <QEvent>
#include <QTime>
#include <QtMath>

#include <QFileDialog>

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
#include <CGAL/boost/graph/copy_face_graph.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>

//#include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>
namespace PMP = CGAL::Polygon_mesh_processing;
using vertex_descriptor = boost::graph_traits<SurfaceMesh_3>::vertex_descriptor;
using face_descriptor = boost::graph_traits<SurfaceMesh_3>::face_descriptor;

PoseSimulation::PoseSimulation( QWidget* mainWindow, Qt3DCore::QEntity* rootEntity, GeographicConvertionWrapper* tmw )
  : mainWindow( mainWindow ), tmw( tmw ) {

  state.setZero();

  setSimulation( false );






  noiseGenerator.seed( std::chrono::system_clock::now().time_since_epoch().count() );

  // test for recording
  {
    m_baseEntity = new Qt3DCore::QEntity( rootEntity );
    m_baseTransform = new Qt3DCore::QTransform( m_baseEntity );
    m_baseEntity->addComponent( m_baseTransform );

    m_pointsEntity = new Qt3DCore::QEntity( m_baseEntity );
    m_terrainEntity = new Qt3DCore::QEntity( m_baseEntity );
    m_linesEntity = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity3 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity4 = new Qt3DCore::QEntity( m_baseEntity );
    m_terrainEntity->setEnabled( false );
    m_linesEntity->setEnabled( false );
    m_segmentsEntity3->setEnabled( false );

    m_pointsMesh = new BufferMesh( m_pointsEntity );
    m_pointsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Points );
    m_pointsEntity->addComponent( m_pointsMesh );

    m_terrainMesh = new BufferMeshWithNormal( m_terrainEntity );
    m_terrainMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Triangles );
    m_terrainEntity->addComponent( m_terrainMesh );

    m_linesMesh = new BufferMesh( m_linesEntity );
    m_linesMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
    m_linesEntity->addComponent( m_linesMesh );

    m_segmentsMesh3 = new BufferMesh( m_segmentsEntity3 );
    m_segmentsMesh3->setPrimitiveType( Qt3DRender::QGeometryRenderer::Points );
    m_segmentsEntity3->addComponent( m_segmentsMesh3 );

    m_segmentsMesh4 = new BufferMesh( m_segmentsEntity4 );
    m_segmentsMesh4->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
    m_segmentsEntity4->addComponent( m_segmentsMesh4 );

    m_pointsMaterial = new Qt3DExtras::QPhongMaterial( m_pointsEntity );
    m_segmentsMaterial = new Qt3DExtras::QPhongMaterial( m_terrainEntity );
    m_segmentsMaterial2 = new Qt3DExtras::QPhongMaterial( m_linesEntity );
    m_segmentsMaterial3 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity3 );
    m_segmentsMaterial4 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity4 );

    m_pointsMaterial->setAmbient( Qt::yellow );
    m_segmentsMaterial->setAmbient( Qt::darkGreen );
    m_segmentsMaterial->setShininess( 0.1f );
    m_segmentsMaterial2->setAmbient( Qt::green );
    m_segmentsMaterial3->setAmbient( Qt::blue );
    m_segmentsMaterial4->setAmbient( Qt::red );

    m_pointsEntity->addComponent( m_pointsMaterial );
    m_terrainEntity->addComponent( m_segmentsMaterial );
    m_linesEntity->addComponent( m_segmentsMaterial2 );
    m_segmentsEntity3->addComponent( m_segmentsMaterial3 );
    m_segmentsEntity4->addComponent( m_segmentsMaterial4 );
  }
}

void PoseSimulation::timerEvent( QTimerEvent* event ) {
  if( event->timerId() == m_timer.timerId() ) {
    constexpr double msPerS = 1000;
    double deltaT = double ( m_time.restart() ) / msPerS;
    double steerAngle = 0;

    if( m_autosteerEnabled ) {
      steerAngle = m_steerAngleFromAutosteer;
    } else {
      steerAngle = m_steerAngle;
    }

    Q_EMIT steeringAngleChanged( steerAngle );
    Q_EMIT velocityChanged( m_velocity );

    auto rollPitchYaw = ( lastFoundFaceOrientation
                          * taitBryanToQuaternion( 0, 0, state( int( ThreeWheeledFRHRL::StateNames::Yaw ) ) )
                        ).conjugate()
                        * taitBryanToQuaternion( 0, 0, state( int( ThreeWheeledFRHRL::StateNames::Yaw ) ) );

    auto taitBryanAngles = quaternionToTaitBryan( rollPitchYaw );

//    state( int( ThreeWheeledFRHRL::StateNames::Roll ) ) = taitBryanAngles.y();
//    state( int( ThreeWheeledFRHRL::StateNames::Pitch ) ) = taitBryanAngles.x();
    {
      Eigen::IOFormat CommaInitFmt( Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "(", ")" );
      std::cout << "taitBryanAngles: " << ( taitBryanAngles * 180 / M_PI ).format( CommaInitFmt ) << std::endl;
    }

    {
      auto& V = m_velocity;
      auto deltaF = qDegreesToRadians( steerAngle );

      {
        constexpr double MinDeltaT = 0.001;
        double factor = std::floor( deltaT / MinDeltaT );
        double deltaT2 = deltaT / factor;

//        std::cout << "factor, deltaT, deltaT2" << factor << deltaT << deltaT2 << std::endl;

        for( int i = 0; i < factor; ++i ) {
          StateType<double> prediction;
//          qDebug() << "PoseSimulation::timerEvent" << Caf << Car << Cah;
          simulatorModel.predict( state, deltaT2,
                                  V, deltaF,
                                  a, b, c,
                                  Caf, Car, Cah,
                                  m, Iz,
                                  sigmaF, sigmaR, sigmaH,
                                  Cx, slipX,
                                  prediction );
          state = prediction;
        }
      }
    }

    {
      if( tin && surfaceMesh ) {
        auto point = Point_3( state( int( ThreeWheeledFRHRL::StateNames::X ) ),
                              state( int( ThreeWheeledFRHRL::StateNames::Y ) ),
                              state( int( ThreeWheeledFRHRL::StateNames::Z ) ) );
        auto foundFace = tin->locate( point, lastFoundFace );
        /*if(lastFoundFace != foundFace) */{

          if( !tin->is_infinite( foundFace ) ) {
            Eigen::Vector3d points[] = {
              toEigenVector( foundFace->vertex( 0 )->point() ),
              toEigenVector( foundFace->vertex( 1 )->point() ),
              toEigenVector( foundFace->vertex( 2 )->point() )
            };

            auto plane = Plane_3( foundFace->vertex( 0 )->point(),
                                  foundFace->vertex( 1 )->point(),
                                  foundFace->vertex( 2 )->point() );
            auto line = Line_3( point, Vector_3( 0, 0, 1 ) );

            auto result = CGAL::intersection( plane, line );

            if( result ) {
              if( auto* const projectedPoint = boost::get<Point_3>( &*result ) ) {
                state( int( ThreeWheeledFRHRL::StateNames::Z ) ) = projectedPoint->z() + 2;
              }
            }

            // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/geometry-of-a-triangle
            auto normalVector = ( points[1] - points[0] ).cross( points[2] - points[0] );
            lastFoundFaceOrientation = Eigen::Quaterniond::FromTwoVectors( Eigen::Vector3d::UnitZ(), normalVector );

            Eigen::IOFormat CommaInitFmt( Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "(", ")" );

            std::cout << "lastFoundFaceOrientation: " << ( quaternionToTaitBryan( lastFoundFaceOrientation ) * 180 / M_PI ).format( CommaInitFmt ) << ", normalVector: " << normalVector.format( CommaInitFmt ) << std::endl;
          } else {
            std::cout << "tin->is_infinite( foundFace )" << std::endl;
            lastFoundFaceOrientation = taitBryanToQuaternion( 0, 0, 0 );
          }

          lastFoundFace = foundFace;

        }
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

      Eigen::IOFormat CommaInitFmt( Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "(", ")" );

      m_orientation = lastFoundFaceOrientation * taitBryanToQuaternion( getYaw( quaternionToTaitBryan( m_orientation ) ), 0, 0 );
      std::cout << "lastFoundFaceOrientation: " << radiansToDegrees( quaternionToTaitBryan( lastFoundFaceOrientation ) ).format( CommaInitFmt ) << ", "
                << "m_orientation: " << radiansToDegrees( quaternionToTaitBryan( m_orientation ) ).format( CommaInitFmt ) << std::endl;

      Q_EMIT orientationChanged( m_orientation );

    }

    {
      /*static uint8_t counter = 0;

      if( ++counter >= 10 )*/ {
//        counter = 0;

        if( noisePositionXYActivated || noisePositionZActivated ) {
          antennaKinematic.setPose( Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::X ) ) + noisePositionXY( noiseGenerator ),
                                    state( int( ThreeWheeledFRHRL::StateNames::Y ) ) + noisePositionXY( noiseGenerator ),
                                    state( int( ThreeWheeledFRHRL::StateNames::Z ) ) + noisePositionZ( noiseGenerator ) ),
                                    m_orientation,
                                    PoseOption::Options() );
        } else {
          antennaKinematic.setPose( Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::X ) ),
                                    state( int( ThreeWheeledFRHRL::StateNames::Y ) ),
                                    state( int( ThreeWheeledFRHRL::StateNames::Z ) ) ),
                                    m_orientation,
                                    PoseOption::Options() );
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
                        antennaKinematic.positionCalculated.z(), latitude, longitude, height );

//      QElapsedTimer timer;
//      timer.start();
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
//      qDebug() << "Cycle Time PoseSimulation:    " << timer.nsecsElapsed() << "ns";
  }
}

void PoseSimulation::toJSON( QJsonObject& json ) {
  QJsonObject valuesObject;

  valuesObject[QStringLiteral( "a" )] = this->a;
  valuesObject[QStringLiteral( "b" )] = this->b;
  valuesObject[QStringLiteral( "c" )] = this->c;
  valuesObject[QStringLiteral( "Caf" )] = this->Caf;
  valuesObject[QStringLiteral( "Car" )] = this->Car;
  valuesObject[QStringLiteral( "Cah" )] = this->Cah;
  valuesObject[QStringLiteral( "m" )] = this->m;
  valuesObject[QStringLiteral( "Iz" )] = this->Iz;
  valuesObject[QStringLiteral( "sigmaF" )] = this->sigmaF;
  valuesObject[QStringLiteral( "sigmaR" )] = this->sigmaR;
  valuesObject[QStringLiteral( "sigmaH" )] = this->sigmaH;
  valuesObject[QStringLiteral( "Cx" )] = this->Cx;
  valuesObject[QStringLiteral( "slip" )] = this->slipX;

  json[QStringLiteral( "values" )] = valuesObject;
}

void PoseSimulation::fromJSON( QJsonObject& json ) {
  if( json[QStringLiteral( "values" )].isObject() ) {
    QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

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
}

void PoseSimulation::setInterval( int interval ) {
  m_interval = interval;

  setSimulation( m_enabled );

  Q_EMIT intervalChanged( m_interval );
}

void PoseSimulation::setSimulation( bool enabled ) {
  m_enabled = enabled;

  if( enabled ) {
    m_timer.start( m_interval, Qt::PreciseTimer, this );
    m_time.start();
  } else {
    m_timer.stop();
  }

  Q_EMIT simulationChanged( m_enabled );
}

void PoseSimulation::setFrequency( double frequency ) {
  setInterval( 1000 / frequency );
}

void PoseSimulation::setSteerAngle( double steerAngle ) {
  m_steerAngle = steerAngle + m_steerAngleOffset;
}

void PoseSimulation::setVelocity( double velocity ) {
  m_velocity = velocity;
}

void PoseSimulation::setRollOffset( double offset ) {
  m_rollOffset = offset;
}

void PoseSimulation::setPitchOffset( double offset ) {
  m_pitchOffset = offset;
}

void PoseSimulation::setYawOffset( double offset ) {
  m_yawOffset = offset;
}

void PoseSimulation::setSteerAngleOffset( double offset ) {
  m_steerAngleOffset = offset;
}

void PoseSimulation::setSteerAngleFromAutosteer( double steerAngle ) {
  m_steerAngleFromAutosteer = steerAngle + m_steerAngleOffset;
}

void PoseSimulation::setInitialWGS84Position( const Eigen::Vector3d& position ) {
  tmw->Reset( position.x(), position.y(), position.z() );
}

void PoseSimulation::autosteerEnabled( bool enabled ) {
  m_autosteerEnabled = enabled;
}

void PoseSimulation::setSimulatorValues( const double a, const double b, const double c, const double Caf, const double Car, const double Cah, const double m, const double Iz, const double sigmaF, const double sigmaR, const double sigmaH, const double Cx, const double slipX ) {
  this->a = a;
  this->b = b;
  this->c = c;
  // N/° -> N/rad
  this->Caf = Caf * 180 / M_PI;
  this->Car = Car * 180 / M_PI;
  this->Cah = Cah * 180 / M_PI;
  this->m = m;
  this->Iz = Iz;
  this->sigmaF = sigmaF;
  this->sigmaR = sigmaR;
  this->sigmaH = sigmaH;
  this->Cx = Cx;
  this->slipX = slipX;
}

void PoseSimulation::setNoiseStandartDeviations( double noisePositionXY, double noisePositionZ, double noiseOrientation, double noiseAccelerometer, double noiseGyro ) {
  if( !qIsNull( noisePositionXY ) ) {
    noisePositionXYActivated = true;
    this->noisePositionXY = std::normal_distribution<double>( 0, noisePositionXY );
  } else {
    noisePositionXYActivated = false;
  }

  if( !qIsNull( noisePositionZ ) ) {
    noisePositionZActivated = true;
    this->noisePositionZ = std::normal_distribution<double>( 0, noisePositionZ );
  } else {
    noisePositionZActivated = false;
  }

  if( !qIsNull( noiseOrientation ) ) {
    noiseOrientationActivated = true;
    this->noiseOrientation = std::normal_distribution<double>( 0, noiseOrientation );
  } else {
    noiseOrientationActivated = false;
  }

  if( !qIsNull( noiseAccelerometer ) ) {
    noiseAccelerometerActivated = true;
    this->noiseAccelerometer = std::normal_distribution<double>( 0, noiseAccelerometer );
  } else {
    noiseAccelerometerActivated = false;
  }

  if( !qIsNull( noiseGyro ) ) {
    noiseGyroActivated = true;
    this->noiseGyro = std::normal_distribution<double>( 0, noiseGyro );
  } else {
    noiseGyroActivated = false;
  }
}

void PoseSimulation::openTIN() {
  QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
  QString dir;

  auto* fileDialog = new QFileDialog( mainWindow,
                                      tr( "Open Saved Field" ),
                                      dir,
                                      selectedFilter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  fileDialog->setNameFilter( tr( "All Files (*);;GeoJSON Files (*.geojson)" ) );

  // connect the signal QFileDialog::urlSelected to a lambda, which opens the file.
  // this is needed, as the file dialog on android is asynchonous, so you have to connect to
  // the signals instead of using the static functions for the dialogs
#ifdef Q_OS_ANDROID
  QObject::connect( fileDialog, &QFileDialog::urlSelected, this, [this, fileDialog]( QUrl fileName ) {
    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile(
              QUrl::fromPercentEncoding(
                      fileName.toString().split( QStringLiteral( "%3A" ) ).at( 1 ).toUtf8() ) );

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
  QObject::connect( fileDialog, &QFileDialog::fileSelected, mainWindow, [this, fileDialog]( const QString & fileName ) {
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

void PoseSimulation::openTINFromFile( QFile& file ) {
  auto geoJsonHelper = GeoJsonHelper( file );
  geoJsonHelper.print();

  std::vector<Point_3> points;
  QVector<QVector3D> pointsForMesh;

  for( const auto& member : geoJsonHelper.members ) {
    switch( member.first ) {
      case GeoJsonHelper::GeometryType::MultiPoint: {
        for( const auto& point : std::get<GeoJsonHelper::MultiPointType>( member.second ) ) {
          auto tmwPoint = tmw->Forward( point );
          pointsForMesh << toQVector3D( tmwPoint );
          points.emplace_back( toPoint3( tmwPoint ) );
        }
      }
      break;

      default:
        break;
    }
  }

  m_pointsMesh->bufferUpdate( pointsForMesh );
  m_pointsEntity->setEnabled( true );

  tin = make_unique<DelaunayTriangulationProjectedXY>( points.cbegin(), points.cend() );
  surfaceMesh = make_unique<SurfaceMesh_3>();
  CGAL::copy_face_graph( *tin, *surfaceMesh );

  auto vnormals = surfaceMesh->add_property_map<vertex_descriptor, Vector_3>( "v:normals", CGAL::NULL_VECTOR ).first;
  auto fnormals = surfaceMesh->add_property_map<face_descriptor, Vector_3>( "f:normals", CGAL::NULL_VECTOR ).first;

  PMP::compute_normals( *surfaceMesh, vnormals, fnormals );
  std::cout << "Vertex normals :" << std::endl;

  for( vertex_descriptor vd : vertices( *surfaceMesh ) ) {
    std::cout << vnormals[vd] << std::endl;
  }

  Eigen::IOFormat CommaInitFmt( Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "(", ")" );

  auto ppm = get( CGAL::vertex_point, *surfaceMesh );

  QVector<QVector3D> trianglesWithNormals;

  std::cout << "Face normals :" << std::endl;

  for( face_descriptor fd : faces( *surfaceMesh ) ) {
    auto normal = fnormals[fd];
    const auto normalVector = toQVector3D( normal );

//    std::vector<QVector3D> points;
    for( vertex_descriptor vd : CGAL::vertices_around_face( surfaceMesh->halfedge( fd ), *surfaceMesh ) ) {
//       points.emplace_back(convertPoint3ToQVector3D( get( ppm, vd ) ));
      trianglesWithNormals << toQVector3D( get( ppm, vd ) );
//      trianglesWithNormals << convertVector3ToQVector3D(vnormals[vd]);
      trianglesWithNormals << normalVector;
    }

    auto quaternion = Eigen::Quaterniond::FromTwoVectors( Eigen::Vector3d( 0, 0, 1 ), toEigenVector( normal ) );

    std::cout << "vector: " << normal << " Euler: " << ( quaternionToTaitBryan( quaternion ) * 180 / M_PI ).format( CommaInitFmt ) << std::endl;
  }

  m_terrainMesh->bufferUpdate( trianglesWithNormals );
  m_terrainEntity->setEnabled( true );


  QVector<QVector3D> lines;

  for( const auto& edge : surfaceMesh->edges() ) {

    auto point1 = surfaceMesh->point( surfaceMesh->vertex( edge, 0 ) );
    auto point2 = surfaceMesh->point( surfaceMesh->vertex( edge, 1 ) );

    std::cout << "line: " << point1 << ", " << point2 << std::endl;

    lines << toQVector3D( point1 );
    lines <<  toQVector3D( point2 );
  }

  m_linesMesh->bufferUpdate( lines );
  m_linesEntity->setEnabled( true );
}

void PoseSimulation::setWheelbase( double wheelbase ) {
  if( !qFuzzyIsNull( wheelbase ) ) {
    m_wheelbase = wheelbase;
  }
}

void PoseSimulation::setAntennaOffset( const Eigen::Vector3d& offset ) {
  auto offsetTmp = offset;
  offsetTmp.x() -= b;
  antennaKinematic.setOffset( -offsetTmp );
}

#include "moc_PoseSimulation.cpp"
