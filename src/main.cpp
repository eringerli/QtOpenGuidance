/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

// tractor-trailer kinematics
// https://userpages.uni-koblenz.de/~zoebel/pdf/Mamotep2.pdf

// axis/rotation conventions
// https://ch.mathworks.com/help/driving/ug/coordinate-systems.html

#include "TractorModel.h"

#include <QGuiApplication>

#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcameralens.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QSlider>
#include <QtGui/QScreen>

#include <Qt3DInput/QInputAspect>

#include <Qt3DExtras/qtorusmesh.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qpointlight.h>

#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qaspectengine.h>

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DExtras/qforwardrenderer.h>

#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/QOrbitCameraController>

#include "cameracontroller.h"

#include "gui/settingsdialog.h"
#include "gui/guidancetoolbar.h"
#include "gui/simulatortoolbar.h"

#include "PoseSimulation.h"
#include "PoseCache.h"
#include "AckermannKinematic.h"

int main( int argc, char** argv ) {
  QApplication app( argc, argv );

  Qt3DExtras::Qt3DWindow* view = new Qt3DExtras::Qt3DWindow();
  view->defaultFrameGraph()->setClearColor( QColor( QRgb( 0x4d4d4f ) ) );
  QWidget* container = QWidget::createWindowContainer( view );
  QSize screenSize = view->screen()->size();
  container->setMinimumSize( QSize( 500, 400 ) );
  container->setMaximumSize( screenSize );

  QWidget* widget = new QWidget;
  QHBoxLayout* hLayout = new QHBoxLayout( widget );
  hLayout->addWidget( container, 1 );



  widget->setWindowTitle( QStringLiteral( "QtOpenGuidance" ) );

  Qt3DInput::QInputAspect* input = new Qt3DInput::QInputAspect;
  view->registerAspect( input );

  // Root entity
  Qt3DCore::QEntity* rootEntity = new Qt3DCore::QEntity();

  // Camera
  Qt3DRender::QCamera* cameraEntity = view->camera();

  cameraEntity->lens()->setPerspectiveProjection( 45.0f, 16.0f / 10.0f, 0.1f, 1000.0f );
  cameraEntity->setPosition( QVector3D( 0, 0, 20.0f ) );
  cameraEntity->setUpVector( QVector3D( 0, 1, 0 ) );
  cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );
  cameraEntity->rollAboutViewCenter( -90 );
  cameraEntity->tiltAboutViewCenter( -45 );

  Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity( rootEntity );
  Qt3DRender::QPointLight* light = new Qt3DRender::QPointLight( lightEntity );
  light->setColor( "white" );
  light->setIntensity( 1 );
  lightEntity->addComponent( light );
  Qt3DCore::QTransform* lightTransform = new Qt3DCore::QTransform( lightEntity );
  lightTransform->setTranslation( cameraEntity->position() );
  lightEntity->addComponent( lightTransform );

  // For camera controls
  CameraController* cameraController = new CameraController(rootEntity, cameraEntity);
  cameraController->setCameraController(2);

  //Qt3DExtras::QOrbitCameraController* camController = new Qt3DExtras::QOrbitCameraController( rootEntity );
  //camController->setCamera( cameraEntity );
  //camController->setLinearSpeed( 150 );

  // TractorEntity
  TractorModel* tractorModelSimulation = new TractorModel( rootEntity );
  TractorModel* tractorModel = new TractorModel( rootEntity );

  // draw an axis-cross: X-red, Y-green, Z-blue
  if( 1 ) {
    Qt3DCore::QEntity* xaxis = new Qt3DCore::QEntity( rootEntity );
    Qt3DCore::QEntity* yaxis = new Qt3DCore::QEntity( rootEntity );
    Qt3DCore::QEntity* zaxis = new Qt3DCore::QEntity( rootEntity );

    Qt3DExtras::QCylinderMesh* cylinderMesh = new Qt3DExtras::QCylinderMesh();
    cylinderMesh->setRadius( 0.05 );
    cylinderMesh->setLength( 20 );
    cylinderMesh->setRings( 2 );
    cylinderMesh->setSlices( 4 );

    Qt3DExtras::QPhongMaterial* blueMaterial = new Qt3DExtras::QPhongMaterial();
    blueMaterial->setSpecular( Qt::white );
    blueMaterial->setShininess( 10 );
    blueMaterial->setAmbient( Qt::blue );

    Qt3DExtras::QPhongMaterial* redMaterial = new Qt3DExtras::QPhongMaterial();
    redMaterial->setSpecular( Qt::white );
    redMaterial->setShininess( 10 );
    redMaterial->setAmbient( Qt::red );

    Qt3DExtras::QPhongMaterial* greenMaterial = new Qt3DExtras::QPhongMaterial();
    greenMaterial->setSpecular( Qt::white );
    greenMaterial->setShininess( 10 );
    greenMaterial->setAmbient( Qt::green );

    Qt3DCore::QTransform* xTransform = new Qt3DCore::QTransform();
    xTransform->setTranslation( QVector3D( 10, 0, 0 ) );
    xTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), 90 ) );
    Qt3DCore::QTransform* yTransform = new Qt3DCore::QTransform();
    yTransform->setTranslation( QVector3D( 0, 10, 0 ) );
    Qt3DCore::QTransform* zTransform = new Qt3DCore::QTransform();
    zTransform->setTranslation( QVector3D( 0, 0, 10 ) );
    zTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 1, 0, 0 ), 90 ) );


    xaxis->addComponent( cylinderMesh );
    xaxis->addComponent( redMaterial );
    xaxis->addComponent( xTransform );
    yaxis->addComponent( cylinderMesh );
    yaxis->addComponent( greenMaterial );
    yaxis->addComponent( yTransform );
    zaxis->addComponent( cylinderMesh );
    zaxis->addComponent( blueMaterial );
    zaxis->addComponent( zTransform );
  }

  // Set root object of the scene
  view->setRootEntity( rootEntity );


  // the processer of Pose etc
  PoseSimulation* poseSimulation = new PoseSimulation();
  PoseCache* poseCache = new PoseCache();
  AckermannKinematic* tractorKinematics = new AckermannKinematic();

  // Toolbars
  SimulatorToolbar* simulatorToolbar = new SimulatorToolbar( widget );
  simulatorToolbar->setVisible(false);
  hLayout->addWidget( simulatorToolbar );

  GuidanceToolbar* guidaceToolbar = new GuidanceToolbar( widget );
  hLayout->addWidget( guidaceToolbar );

  // Create setting Window
  SettingsDialog* settingDialog = new SettingsDialog( widget );


  // GUI -> GUI
  QObject::connect( guidaceToolbar, SIGNAL( simulatorChanged( bool ) ),
                    simulatorToolbar, SLOT( setVisible( bool ) ) );
  QObject::connect( guidaceToolbar, SIGNAL( toggleSettings(  ) ),
                    settingDialog, SLOT( toggleVisibility(  ) ) );

  // GUI -> Simulator
  QObject::connect( guidaceToolbar, SIGNAL( simulatorChanged( bool ) ),
                    poseSimulation, SLOT( setSimulation( bool ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( velocityChanged( float ) ),
                    poseSimulation, SLOT( setVelocity( float ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( frequencyChanged( int ) ),
                    poseSimulation, SLOT( setFrequency( int ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( steerangleChanged( float ) ),
                    poseSimulation, SLOT( setSteerAngle( float ) ) );


  // Settings -> Simulator
  QObject::connect( settingDialog, SIGNAL( wheelbaseChanged( float ) ),
                    poseSimulation, SLOT( setWheelbase( float ) ) );
  QObject::connect( settingDialog, SIGNAL( antennaPositionChanged( QVector3D ) ),
                    poseSimulation, SLOT( setAntennaPosition( QVector3D ) ) );

  // Simulator -> PoseCache
  QObject::connect( poseSimulation, SIGNAL( positionChanged( QVector3D ) ),
                    poseCache, SLOT( setPosition( QVector3D ) ) );
  QObject::connect( poseSimulation, SIGNAL( orientationChanged( QQuaternion ) ),
                    poseCache, SLOT( setOrientation( QQuaternion ) ) );
  QObject::connect( poseSimulation, SIGNAL( steeringAngleChanged( float ) ),
                    poseCache, SLOT( setSteeringAngle( float ) ) );

  // PoseCache -> Tractor Model
  QObject::connect( poseCache, SIGNAL( poseChanged( QVector3D, QQuaternion, float ) ),
                    tractorModelSimulation, SLOT( setPose( QVector3D, QQuaternion, float ) ) );

  // PoseCache -> Tractor Kinematics
  QObject::connect( poseCache, SIGNAL( poseChanged( QVector3D, QQuaternion, float ) ),
                    tractorKinematics, SLOT( setPose( QVector3D, QQuaternion, float ) ) );

  // Tractor Kinematics -> Tractor Model
  QObject::connect( tractorKinematics, SIGNAL( poseChanged( QVector3D, QQuaternion, float ) ),
                    tractorModel, SLOT( setPose( QVector3D, QQuaternion, float ) ) );
  QObject::connect( tractorKinematics, SIGNAL( hitchPositionChanged( QVector3D ) ),
                    tractorModel, SLOT( setHitchPosition( QVector3D ) ) );
  QObject::connect( tractorKinematics, SIGNAL( towPointPositionChanged( QVector3D ) ),
                    tractorModel, SLOT( setAntennaPosition( QVector3D ) ) );



  //  Settings -> Tractor Model
    QObject::connect( settingDialog, SIGNAL( antennaPositionChanged( QVector3D ) ),
                      tractorModelSimulation, SLOT( setAntennaPosition( QVector3D ) ) );
    QObject::connect( settingDialog, SIGNAL( wheelbaseChanged( float ) ),
                      tractorModelSimulation, SLOT( setWheelbase( float ) ) );
    QObject::connect( settingDialog, SIGNAL( hitchPositionChanged( QVector3D ) ),
                      tractorModelSimulation, SLOT( setHitchPosition( QVector3D ) ) );

  QObject::connect( settingDialog, SIGNAL( antennaPositionChanged( QVector3D ) ),
                    tractorKinematics, SLOT( setTowPointPosition( QVector3D ) ) );
  QObject::connect( settingDialog, SIGNAL( wheelbaseChanged( float ) ),
                    tractorKinematics, SLOT( setWheelbase( float ) ) );
  QObject::connect( settingDialog, SIGNAL( hitchPositionChanged( QVector3D ) ),
                    tractorKinematics, SLOT( setOwnHitch( QVector3D ) ) );


  // Camerastuff
  QObject::connect( settingDialog, SIGNAL( cameraChanged( int ) ),
                    cameraController, SLOT( setCameraController( int ) ) );

  QObject::connect( poseSimulation, SIGNAL( positionChangedRelative( QVector3D ) ),
                    cameraEntity, SLOT( translateWorld( QVector3D ) ) );
  QObject::connect( poseSimulation, SIGNAL( positionChanged( QVector3D ) ),
                    cameraEntity, SLOT( setViewCenter( QVector3D ) ) );

  QObject::connect( cameraEntity, SIGNAL( positionChanged( QVector3D ) ),
                    lightTransform, SLOT( setTranslation( QVector3D ) ) );

  QObject::connect( poseSimulation, SIGNAL( orientationChangedRelative( QQuaternion ) ),
                    cameraEntity, SLOT( rotateAboutViewCenter( QQuaternion ) ) );

  // Show window
  widget->show();
  widget->resize( 1200, 800 );

  return app.exec();
}

