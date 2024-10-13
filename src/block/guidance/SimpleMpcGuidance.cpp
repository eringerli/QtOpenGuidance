// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SimpleMpcGuidance.h"

#include "block/dock/plot/XyPlotDockBlock.h"
#include "filter/SlewRateLimiter.h"
#include "gui/MyMainWindow.h"
#include "gui/dock/PlotDock.h"
#include "helpers/anglesHelper.h"
#include "qcustomplot.h"
#include <QMenu>
#include <limits>

#include "3d/BufferMesh.h"

#include "qentity.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"
#include "qphongmaterial.h"

#include "filter/SlewRateLimiter.h"

void
SimpleMpcGuidance::setWheelbase( const double wheelbase, CalculationOption::Options options ) {
  this->wheelbase = wheelbase;
}

void
SimpleMpcGuidance::setTrackwidth( const double trackwidth, CalculationOption::Options options ) {
  this->trackwidth = trackwidth;
}

void
SimpleMpcGuidance::setAntennaPosition( const Eigen::Vector3d& offset, CalculationOption::Options ) {
  antennaKinematic.setOffset( -offset );
}

void
SimpleMpcGuidance::setWindow( const double window, CalculationOption::Options options ) {
  if( window > 2 && window < 100 ) {
    this->window = window;
  }
}

void
SimpleMpcGuidance::setSteps( const double steps, CalculationOption::Options options ) {
  if( steps > 3 && steps < 100 ) {
    this->steps = steps;
  }
}

void
SimpleMpcGuidance::setSteerAngleThreshold( const double steerAngleThreshold, const CalculationOption::Options options ) {
  this->steerAngleThresholdRad = degreesToRadians( steerAngleThreshold );
}

void
SimpleMpcGuidance::setWeight( const double weight, const CalculationOption::Options ) {
  if( weight > 0.2 || weight < -0.2 ) {
    this->weight = weight;
  }
}

void
SimpleMpcGuidance::setMaxSlewRateSteering( const double maxSlewRateSteeringDegreesPerSec, const CalculationOption::Options ) {
  this->maxSlewRateSteeringRadPerSec = degreesToRadians( maxSlewRateSteeringDegreesPerSec );
}

void
SimpleMpcGuidance::setPlan( const Plan& plan ) {
  this->plan         = plan;
  lastFoundPrimitive = plan.plan->cend();
}

void
SimpleMpcGuidance::setSteerAngle( const double steerAngle, CalculationOption::Options options ) {
  this->steerAngleRadians = degreesToRadians( steerAngle );
}

void
SimpleMpcGuidance::setVelocity( const double velocity, CalculationOption::Options options ) {
  this->velocity = velocity;
}

double
SimpleMpcGuidance::calculateWeightFactor( const double index, const double size ) {
  if( weight < .95 || weight > 1.05 ) {
    if( weight > 0. ) {
      return std::pow( weight / size * ( index + 1 ), 2 );
    } else {
      return std::pow( weight / size * ( size - index ), 2 );
    }
  }

  return weight;
}

double
SimpleMpcGuidance::runMpc( const double           dT,
                           const double           steeringAngleToCalculateWithRadians,
                           const double           yawToStartFromRadians,
                           const Eigen::Vector3d& positionToStartFrom ) {
  this->recordXteTo = std::make_shared< std::vector< double > >();
  this->recordXteTo->reserve( steps );

  this->recordHeadingTo = std::make_shared< std::vector< double > >();
  this->recordHeadingTo->reserve( steps );

  this->recordHeadingOfPathTo = std::make_shared< std::vector< double > >();
  this->recordHeadingOfPathTo->reserve( steps );

  //  std::vector< double > yawRadDifference;
  //  yawRadDifference.reserve( steps );

  //  for( int step = 0; step < steps; ++step ) {

  //  }

  //  double yawRadDifference = dT * velocity * std::tan( steeringAngleToCalculateWithRadians ) / wheelbase;

  Eigen::Vector3d position = positionToStartFrom;
  double          yawRad   = yawToStartFromRadians;

  Q_EMIT triggerPose(
    Eigen::Vector3d( 0, 0, 0 ), Eigen::Quaterniond( 0, 0, 0, 0 ), CalculationOption::Option::RestoreStateToBeforeMpcCalculation );

  double dtVelocity = dT * velocity;

  SlewRateLimiter< double > slewRateLimiter( maxSlewRateSteeringRadPerSec );
  slewRateLimiter.previousValue = steerAngleRadians;

  for( int step = 0; step < steps; ++step ) {
    double yawRadDifference = dT * velocity * std::tan( slewRateLimiter.set( steeringAngleToCalculateWithRadians, dT ) ) / wheelbase;

    Eigen::Vector3d positionDifference( dtVelocity * std::cos( yawRad ), dtVelocity * std::sin( yawRad ), 0 );
    position = position + positionDifference;
    yawRad   = normalizeAngleRadians( yawRad + yawRadDifference );

    auto taitBryan      = Eigen::Vector3d( 0, 0, 0 );
    getYaw( taitBryan ) = yawRad;
    auto quat           = taitBryanToQuaternion( taitBryan );

    Q_EMIT triggerPose( position, quat, CalculationOption::Option::CalculateMpcUpdate );
  }

  double costFunctionResult = 0;
  for( int i = 0; i < recordXteTo->size(); ++i ) {
    double xteFactor = calculateWeightFactor( i, recordXteTo->size() );
    costFunctionResult += recordXteTo->at( i ) * xteFactor;
    //    qDebug() << "heading: " << recordHeadingTo->at( i ) - degreesToRadians( recordHeadingOfPathTo->at( i ) ) << recordHeadingTo->at( i
    //    )
    //             << degreesToRadians( recordHeadingOfPathTo->at( i ) );

    //    costFunctionResult += recordHeadingTo->at( i ) + recordHeadingOfPathTo->at( i );
  }

  return std::abs( costFunctionResult );
}

void
SimpleMpcGuidance::goldenRatioMpc( const Eigen::Vector3d&                    positionPose,
                                   const Eigen::Quaterniond&                 orientationPose,
                                   const double                              dT,
                                   const double                              yawToStartFromRadians,
                                   std::shared_ptr< std::vector< double > >& steeringAngles,
                                   std::shared_ptr< std::vector< double > >& xtesCostFunctionResults ) {
  const double goldenRatio = ( std::sqrt( 5. ) - 1. ) / 2.;

  double steerAngleMax = degreesToRadians( 35. );

  double xU = steerAngleMax;
  double xL = -steerAngleMax;
  double x1 = xL + goldenRatio * ( xU - xL );
  double x2 = xU - goldenRatio * ( xU - xL );

  double cost1 = runMpc( dT, x1, yawToStartFromRadians, positionPose );
  //  qDebug() << "SimpleMpcGuidance::setPose" << 0 << radiansToDegrees( x1 ) << cost1;

  steeringAngles->push_back( x1 );
  xtesCostFunctionResults->push_back( cost1 );

  double cost2 = runMpc( dT, x2, yawToStartFromRadians, positionPose );
  //  qDebug() << "SimpleMpcGuidance::setPose" << 0 << radiansToDegrees( x2 ) << cost2;

  steeringAngles->push_back( x2 );
  xtesCostFunctionResults->push_back( cost2 );

  for( int i = 0; i < 8; ++i ) {
    //      qDebug() << xL << x1 << x2 << xU;
    if( cost2 < cost1 ) {
      //      qDebug() << "SimpleMpcGuidance::setPose >" << i << radiansToDegrees( x1 ) << radiansToDegrees( x2 ) << cost1 << cost2;
      xU    = x1;
      x1    = x2;
      cost1 = cost2;

      x2 = xU - goldenRatio * ( xU - xL );
      if( std::abs( x2 ) < degreesToRadians( 0.2 ) ) {
        x2 = 0.;
      }

      cost2 = runMpc( dT, x2, yawToStartFromRadians, positionPose );

      steeringAngles->push_back( x2 );
      xtesCostFunctionResults->push_back( cost2 );
    } else {
      //      qDebug() << "SimpleMpcGuidance::setPose <" << i << radiansToDegrees( x1 ) << radiansToDegrees( x2 ) << cost1 << cost2;
      xL    = x2;
      x2    = x1;
      cost2 = cost1;

      x1 = xL + goldenRatio * ( xU - xL );
      if( std::abs( x1 ) < degreesToRadians( 0.2 ) ) {
        x1 = 0.;
      }

      cost1 = runMpc( dT, x1, yawToStartFromRadians, positionPose );

      steeringAngles->push_back( x1 );
      xtesCostFunctionResults->push_back( cost1 );
    }
  }

  if( cost2 < cost1 ) {
    //    qDebug() << "SimpleMpcGuidance::steerAngleChanged >" << radiansToDegrees( x1 ) << radiansToDegrees( x2 ) << cost1 << cost2;

    steeringAngles->push_back( x2 );
    xtesCostFunctionResults->push_back( cost2 );

    Q_EMIT triggerPose( positionPose, orientationPose, CalculationOption::Option::RestoreStateToBeforeMpcCalculation );
    Q_EMIT steerAngleChanged( radiansToDegrees( x2 ), CalculationOption::Option::None );
  } else {
    //    qDebug() << "SimpleMpcGuidance::steerAngleChanged <" << radiansToDegrees( x1 ) << radiansToDegrees( x2 ) << cost1 << cost2;

    steeringAngles->push_back( x1 );
    xtesCostFunctionResults->push_back( cost1 );

    Q_EMIT triggerPose( positionPose, orientationPose, CalculationOption::Option::RestoreStateToBeforeMpcCalculation );
    Q_EMIT steerAngleChanged( radiansToDegrees( x1 ), CalculationOption::Option::None );
  }
}

bool
SimpleMpcGuidance::newtonMethodMpc( const Eigen::Vector3d&                    positionPose,
                                    const Eigen::Quaterniond&                 orientationPose,
                                    const double                              dT,
                                    const double                              yawToStartFromRadians,
                                    const double                              steeringAngleToStartFrom,
                                    std::shared_ptr< std::vector< double > >& steeringAngles,
                                    std::shared_ptr< std::vector< double > >& xtesCostFunctionResults ) {
  double steeringAngleToCalculateWithRadians = steeringAngleToStartFrom;

  for( int i = 0;; ++i ) {
    double costFunctionResult = runMpc( dT, steeringAngleToCalculateWithRadians, yawToStartFromRadians, positionPose );
    steeringAngles->push_back( steeringAngleToCalculateWithRadians );
    xtesCostFunctionResults->push_back( costFunctionResult );

    double costDerivative = ( *( xtesCostFunctionResults->cend() - 1 ) - *( xtesCostFunctionResults->cend() - 2 ) ) /
                            ( *( steeringAngles->cend() - 1 ) - *( steeringAngles->cend() - 2 ) );

    // first calculation run
    if( i == 0 ) {
      if( recordXteTo && !recordXteTo->empty() ) {
        steeringAngleToCalculateWithRadians += degreesToRadians( ( recordXteTo->front() < 0. ) ? -1. : 1. );
      }
    }

    if( i >= 1 ) {
      double steeringAngleFuture = steeringAngleToCalculateWithRadians - ( costFunctionResult / costDerivative );

      if( std::abs( steeringAngleFuture ) > degreesToRadians( 35. ) ) {
        return false;
      }

      steeringAngleToCalculateWithRadians = steeringAngleFuture;

      if( i >= 2 && std::abs( *( steeringAngles->cend() - 1 ) - *( steeringAngles->cend() - 2 ) ) < steerAngleThresholdRad ) {
        //        qDebug() << "SimpleMpcGuidance::newtonMethodMpc" << i << radiansToDegrees( steeringAngles->back() )
        //                 << radiansToDegrees( *( steeringAngles->cend() - 1 ) - *( steeringAngles->cend() - 2 ) ) << costFunctionResult
        //                 << costDerivative;
        Q_EMIT steerAngleChanged( radiansToDegrees( steeringAngles->back() ), CalculationOption::Option::None );
        return true;
      }

      if( i > 8 ) {
        //        qDebug() << "SimpleMpcGuidance::newtonMethodMpc false";
        return false;
      }
    }
  }
  return true;
}

void
SimpleMpcGuidance::setPose( const Eigen::Vector3d&           positionPose,
                            const Eigen::Quaterniond&        orientationPose,
                            const CalculationOption::Options options ) {
  if( options.testFlag( CalculationOption::CalculateMpcUpdate ) ) {
  }

  if( !options.testFlag( CalculationOption::CalculateMpcUpdate ) &&
      !options.testFlag( CalculationOption::RestoreStateToBeforeMpcCalculation ) && velocity > 0.1 && !plan.plan->empty() ) {
    QElapsedTimer timer;
    timer.start();

    recordPointsTo       = std::make_shared< std::vector< Eigen::Vector3d > >();
    recordXteForPointsTo = std::make_shared< std::vector< double > >();

    auto steeringAngles          = std::make_shared< std::vector< double > >();
    auto xtesCostFunctionResults = std::make_shared< std::vector< double > >();

    auto taitBryanOrientationPose = quaternionToTaitBryan( orientationPose );
    auto yawToStartFromRadians    = getYaw( taitBryanOrientationPose );

    double dT = window / velocity / steps;
    if( !newtonMethodMpc(
          positionPose, orientationPose, dT, yawToStartFromRadians, steerAngleRadians, steeringAngles, xtesCostFunctionResults ) ) {
      goldenRatioMpc( positionPose, orientationPose, dT, yawToStartFromRadians, steeringAngles, xtesCostFunctionResults );
    }

    Q_EMIT dockValuesChanged( steeringAngles, xtesCostFunctionResults );

    QVector< QVector3D > points;
    QVector< QVector3D > points2;
    points.reserve( recordPointsTo->size() * 2 );
    points2.reserve( recordPointsTo->size() );
    QVector3D leftLowerPoint( -0.1f, -0.2f, 0. );
    QVector3D rightLowerPoint( 0.1f, -0.2f, 0. );
    for( const auto& point : *recordPointsTo ) {
      points2.push_back( toQVector3D( point ) );
      points2.push_back( toQVector3D( point ) + leftLowerPoint );
      points2.push_back( toQVector3D( point ) + rightLowerPoint );
    }
    double t = 0;
    for( int i = 0; i < recordPointsTo->size(); ++i ) {
      points.push_back( toQVector3D( recordPointsTo->at( i ) ) );
      points.push_back( toQVector3D( recordPointsTo->at( i ) ) -
                        QVector3D( 0, recordXteForPointsTo->at( i ) * calculateWeightFactor( i, recordXteForPointsTo->size() ), 0 ) );
      t += dT;
      if( dT > window ) {
        t = 0;
      }
    }
    Q_EMIT bufferChanged( points );
    Q_EMIT buffer2Changed( points2 );

    double processingTime = double( timer.nsecsElapsed() ) / 1.e6;
    //    qDebug() << "Cycle Time SimpleMpcGuidance [ms]:" << Qt::fixed << qSetRealNumberPrecision( 4 ) << qSetFieldWidth( 7 ) <<
    //    processingTime;
  }
}

void
SimpleMpcGuidance::setXTE( const double xte, CalculationOption::Options options ) {
  if( options.testFlag( CalculationOption::CalculateMpcUpdate ) ) {
    if( recordXteTo ) {
      recordXteTo->push_back( xte );
    }
    if( recordXteForPointsTo ) {
      recordXteForPointsTo->push_back( xte );
    }
  }
}

void
SimpleMpcGuidance::setHeadingOfPath( const double headingOfPath, const CalculationOption::Options option ) {
  if( option.testFlag( CalculationOption::CalculateMpcUpdate ) ) {
    if( recordHeadingOfPathTo ) {
      recordHeadingOfPathTo->push_back( headingOfPath );
    }
  }
}

void
SimpleMpcGuidance::setPoseResult( const Eigen::Vector3d&           position,
                                  const Eigen::Quaterniond&        orientation,
                                  const CalculationOption::Options option ) {
  if( option.testFlag( CalculationOption::CalculateMpcUpdate ) ) {
    if( recordPointsTo ) {
      recordPointsTo->push_back( position );
    }
    if( recordHeadingTo ) {
      recordHeadingTo->push_back( getYaw( quaternionToTaitBryan( orientation ) ) );
    }
  }
}

void
SimpleMpcGuidance::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT steerAngleChanged( 0, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
SimpleMpcGuidanceFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< SimpleMpcGuidance >( idHint );

  auto* obj2 = new XyPlotDockBlock( mainWindow, getNameOfFactory() + QString::number( obj->id() + 10000 ), 0, false, "XyPlotDockBlock" );
  obj->addAdditionalObject( obj2 );

  obj2->dock->setTitle( getNameOfFactory() );
  obj2->dock->setWidget( obj2->widget );

  menu->addAction( obj2->dock->toggleAction() );

  if( PlotDockBlockBase::firstPlotDock == nullptr ) {
    mainWindow->addDockWidget( obj2->dock, location );
    PlotDockBlockBase::firstPlotDock = obj2->dock;
  } else {
    mainWindow->addDockWidget( obj2->dock, KDDockWidgets::Location_OnBottom, PlotDockBlockBase::firstPlotDock );
  }

  QObject::connect(
    obj2->widget->getQCustomPlotWidget(), &QCustomPlot::mouseDoubleClick, obj2, &PlotDockBlockBase::qCustomPlotWidgetMouseDoubleClick );

  QObject::connect( obj.get(), &SimpleMpcGuidance::dockValuesChanged, obj2, &XyPlotDockBlock::setAngleCost );

  {
    auto entity = new Qt3DCore::QEntity( rootEntity );
    auto mesh   = new BufferMesh( entity );
    mesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
    entity->addComponent( mesh );
    auto material = new Qt3DExtras::QPhongMaterial( entity );
    material->setAmbient( Qt::green );
    material->setDiffuse( Qt::green );
    entity->addComponent( material );
    QObject::connect( obj.get(), &SimpleMpcGuidance::bufferChanged, mesh, &BufferMesh::bufferUpdate );
  }
  {
    auto entity = new Qt3DCore::QEntity( rootEntity );
    auto mesh   = new BufferMesh( entity );
    mesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Triangles );
    entity->addComponent( mesh );
    auto material = new Qt3DExtras::QPhongMaterial( entity );
    material->setAmbient( Qt::red );
    material->setDiffuse( Qt::red );
    entity->addComponent( material );
    QObject::connect( obj.get(), &SimpleMpcGuidance::buffer2Changed, mesh, &BufferMesh::bufferUpdate );
  }

  obj->addInputPort( QStringLiteral( "Wheelbase" ), obj.get(), QLatin1StringView( SLOT( setWheelbase( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Trackwidth" ), obj.get(), QLatin1StringView( SLOT( setTrackwidth( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Antenna Position" ), obj.get(), QLatin1StringView( SLOT( setAntennaPosition( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Window" ), obj.get(), QLatin1StringView( SLOT( setWindow( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Steps" ), obj.get(), QLatin1StringView( SLOT( setSteps( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Weight" ), obj.get(), QLatin1StringView( SLOT( setWeight( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Steering Max Slew Rate" ), obj.get(), QLatin1StringView( SLOT( setMaxSlewRateSteering( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Steer Angle Threshold" ), obj.get(), QLatin1StringView( SLOT( setSteerAngleThreshold( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Plan" ), obj.get(), QLatin1StringView( SLOT( setPlan( const Plan& ) ) ) );
  obj->addInputPort( QStringLiteral( "Steer Angle" ), obj.get(), QLatin1StringView( SLOT( setSteerAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Velocity" ), obj.get(), QLatin1StringView( SLOT( setVelocity( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Trigger Pose" ), obj.get(), QLatin1StringView( SIGNAL( triggerPose( POSE_SIGNATURE_SIGNAL ) ) ) );

  obj->addInputPort( QStringLiteral( "XTE" ), obj.get(), QLatin1StringView( SLOT( setXTE( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Heading Of Path" ), obj.get(), QLatin1StringView( SLOT( setHeadingOfPath( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose Result" ), obj.get(), QLatin1StringView( SLOT( setPoseResult( POSE_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Steer Angle" ), obj.get(), QLatin1StringView( SIGNAL( steerAngleChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  return obj;
}
