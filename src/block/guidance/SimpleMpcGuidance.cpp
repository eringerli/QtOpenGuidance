// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SimpleMpcGuidance.h"

#include "block/dock/plot/MpcPlotDockBlock.h"
#include "gui/MyMainWindow.h"
#include "gui/dock/PlotDock.h"
#include "qcustomplot.h"
#include <QMenu>

#include "3d/BufferMesh.h"

#include "qentity.h"
#include "qneblock.h"
#include "qneport.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"
#include "qphongmaterial.h"

void
SimpleMpcGuidance::setWheelbase( const double wheelbase, CalculationOption::Options options ) {
  this->wheelbase = wheelbase;
}

void
SimpleMpcGuidance::setTrackwidth( const double trackwidth, CalculationOption::Options options ) {
  this->trackwidth = trackwidth;
}

void
SimpleMpcGuidance::setAntennaPosition( const Eigen::Vector3d& offset ) {
  antennaKinematic.setOffset( -offset );
}

void
SimpleMpcGuidance::setWindow( const double window, CalculationOption::Options options ) {
  this->window = window;
}

void
SimpleMpcGuidance::setSteps( const double steps, CalculationOption::Options options ) {
  this->steps = steps;
}

void
SimpleMpcGuidance::setSteerAngleThreshold( const double steerAngleThreshold, const CalculationOption::Options options ) {
  this->steerAngleThresholdRad = degreesToRadians( steerAngleThreshold );
}

void
SimpleMpcGuidance::setWeight( const double weight, const CalculationOption::Options ) {
  this->weight = weight;
}

void
SimpleMpcGuidance::setPlan( const Plan& plan ) {
  this->plan         = plan;
  lastFoundPrimitive = plan.plan->cend();
}

void
SimpleMpcGuidance::setSteerAngle( const double steerAngle, CalculationOption::Options options ) {
  this->steerAngle = steerAngle;
}

void
SimpleMpcGuidance::setVelocity( const double velocity, CalculationOption::Options options ) {
  this->velocity = velocity;
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

  double yawRadDifference = dT * velocity * std::tan( steeringAngleToCalculateWithRadians ) / wheelbase;

  Eigen::Vector3d position = positionToStartFrom;
  double          yawRad   = yawToStartFromRadians;

  Q_EMIT triggerPose(
    Eigen::Vector3d( 0, 0, 0 ), Eigen::Quaterniond( 0, 0, 0, 0 ), CalculationOption::Option::RestoreStateToBeforeMpcCalculation );

  double dtVelocity = dT * velocity;

  for( int step = 0; step < steps; ++step ) {
    Eigen::Vector3d positionDifference( dtVelocity * std::cos( yawRad ), dtVelocity * std::sin( yawRad ), 0 );
    position = position + positionDifference;
    yawRad   = normalizeAngleRadians( yawRad + yawRadDifference );

    auto taitBryan      = Eigen::Vector3d( 0, 0, 0 );
    getYaw( taitBryan ) = yawRad;
    auto quat           = taitBryanToQuaternion( taitBryan );

    Q_EMIT triggerPose( position, quat, CalculationOption::Option::CalculateMpcUpdate );
  }

  double costFunctionResult = 0;
  double t                  = 0;
  for( int i = 0; i < recordXteTo->size(); ++i ) {
    costFunctionResult += recordXteTo->at( i ) * ( 1 + std::pow( t, 2 ) / weight );
    t += dT;
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

    double steeringAngleToCalculateWithRadians = degreesToRadians( steerAngle );

    double dT = window / velocity / steps;
    if( !newtonMethodMpc( positionPose,
                          orientationPose,
                          dT,
                          yawToStartFromRadians,
                          steeringAngleToCalculateWithRadians,
                          steeringAngles,
                          xtesCostFunctionResults ) ) {
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
      points.push_back( toQVector3D( recordPointsTo->at( i ) ) - QVector3D( 0, recordXteForPointsTo->at( i ), 0 ) / weight );
      t += dT;
      if( dT > window ) {
        t = 0;
      }
    }
    Q_EMIT bufferChanged( points );
    Q_EMIT buffer2Changed( points2 );

    double processingTime = double( timer.nsecsElapsed() ) / 1.e6;
    qDebug() << "Cycle Time SimpleMpcGuidance [ms]:" << Qt::fixed << qSetRealNumberPrecision( 4 ) << qSetFieldWidth( 7 ) << processingTime;
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
  Q_EMIT steerAngleChanged( std::numeric_limits< double >::infinity(), CalculationOption::Option::None );
}

QNEBlock*
SimpleMpcGuidanceFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new SimpleMpcGuidance();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  auto* obj2 = new MpcPlotDockBlock( getNameOfFactory() + QString::number( id ), mainWindow );
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

  QObject::connect( obj, &SimpleMpcGuidance::dockValuesChanged, obj2, &MpcPlotDockBlock::setValues );

  {
    auto entity = new Qt3DCore::QEntity( rootEntity );
    auto mesh   = new BufferMesh( entity );
    mesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
    entity->addComponent( mesh );
    auto material = new Qt3DExtras::QPhongMaterial( entity );
    material->setAmbient( Qt::green );
    material->setDiffuse( Qt::green );
    entity->addComponent( material );
    QObject::connect( obj, &SimpleMpcGuidance::bufferChanged, mesh, &BufferMesh::bufferUpdate );
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
    QObject::connect( obj, &SimpleMpcGuidance::buffer2Changed, mesh, &BufferMesh::bufferUpdate );
  }

  b->addInputPort( QStringLiteral( "Wheelbase" ), QLatin1String( SLOT( setWheelbase( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Trackwidth" ), QLatin1String( SLOT( setTrackwidth( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Antenna Position" ), QLatin1String( SLOT( setAntennaPosition( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Window" ), QLatin1String( SLOT( setWindow( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Steps" ), QLatin1String( SLOT( setSteps( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Weight" ), QLatin1String( SLOT( setWeight( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Steer Angle Threshold" ), QLatin1String( SLOT( setSteerAngleThreshold( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );
  b->addInputPort( QStringLiteral( "Steer Angle" ), QLatin1String( SLOT( setSteerAngle( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "XTE" ), QLatin1String( SLOT( setXTE( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Heading Of Path" ), QLatin1String( SLOT( setHeadingOfPath( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Result" ), QLatin1String( SLOT( setPoseResult( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Trigger Pose" ), QLatin1String( SIGNAL( triggerPose( POSE_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Steer Angle" ), QLatin1String( SIGNAL( steerAngleChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  return b;
}
