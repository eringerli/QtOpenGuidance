// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QPointer>

#include "block/BlockBase.h"

#include "3d/qt3dForwards.h"

#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>

#include "helpers/eigenHelper.h"

class MyMainWindow;
class Implement;
class QLabel;

class SectionControl : public BlockBase {
  Q_OBJECT

public:
  explicit SectionControl( const QString&               uniqueName,
                           MyMainWindow*                mainWindow,
                           Qt3DCore::QEntity*           rootEntity,
                           Qt3DRender::QFrameGraphNode* frameGraphParent );
  ~SectionControl();

public Q_SLOTS:
  void setPose( POSE_SIGNATURE_SLOT );
  void setImplement( const QPointer< Implement >& );
  void setSections();
  void setLayer( Qt3DRender::QLayer* );

  void onImageRenderedTurnOnTexture();
  void onImageRenderedTurnOffTexture();

private:
  void requestRenderCaptureTurnOnTexture();
  void requestRenderCaptureTurnOffTexture();

Q_SIGNALS:

public:
  KDDockWidgets::DockWidget* dock                = nullptr;
  QLabel*                    labelTurnOnTexture  = nullptr;
  QLabel*                    labelTurnOffTexture = nullptr;

private:
  Qt3DRender::QFrameGraphNode* frameGraphParent = nullptr;
  Qt3DRender::QFrameGraphNode* frameGraph       = nullptr;
  Qt3DRender::QLayerFilter*    layerFilter      = nullptr;

  Qt3DRender::QCamera* cameraEntityTurnOnTexture = nullptr;
  QVector3D            cameraOffsetTurnOnTexture = QVector3D( 0, 0, 50 );

  TextureRenderTarget*        textureTargetTurnOnTexture = nullptr;
  Qt3DRender::QRenderCapture* renderCaptureTurnOnTexture = nullptr;
  //  Qt3DRender::QRenderCaptureReply* replyTurnOnTexture         = nullptr;

  Qt3DRender::QCamera* cameraEntityTurnOffTexture = nullptr;
  QVector3D            cameraOffsetTurnOffTexture = QVector3D( 0, 0, 50 );

  TextureRenderTarget*        textureTargetTurnOffTexture = nullptr;
  Qt3DRender::QRenderCapture* renderCaptureTurnOffTexture = nullptr;
  //  Qt3DRender::QRenderCaptureReply* replyTurnOffTexture         = nullptr;

  QPointer< Implement > implement;

  std::vector< double >   sectionOffsets;
  std::vector< uint16_t > sectionPixelOffsets;
};

class SectionControlFactory : public BlockFactory {
  Q_OBJECT

public:
  SectionControlFactory( QThread*                     thread,
                         MyMainWindow*                mainWindow,
                         KDDockWidgets::Location      location,
                         QMenu*                       menu,
                         Qt3DCore::QEntity*           rootEntity,
                         Qt3DRender::QFrameGraphNode* frameGraphParent )
      : BlockFactory( thread, false )
      , mainWindow( mainWindow )
      , location( location )
      , menu( menu )
      , rootEntity( rootEntity )
      , frameGraphParent( frameGraphParent ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Section Control" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Section Control" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  MyMainWindow*                mainWindow = nullptr;
  KDDockWidgets::Location      location;
  QMenu*                       menu             = nullptr;
  Qt3DCore::QEntity*           rootEntity       = nullptr;
  Qt3DRender::QFrameGraphNode* frameGraphParent = nullptr;
};
