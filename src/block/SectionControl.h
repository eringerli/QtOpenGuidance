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

#include <QObject>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include "../3d/texturerendertarget.h"
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QRenderCaptureReply>

#include "BlockBase.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

#include "../block/Implement.h"

class SectionControl : public BlockBase {
    Q_OBJECT

  public:
    explicit SectionControl( const QString& uniqueName,
                             MyMainWindow* mainWindow,
                             Qt3DCore::QEntity* rootEntity,
                             Qt3DRender::QFrameGraphNode* frameGraphParent );
    ~SectionControl();

  public slots:
    void setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options );
    void setImplement( const QPointer<Implement>& );
    void setSections();
    void setLayer( Qt3DRender::QLayer* );

    void onImageRenderedTurnOnTexture();
    void onImageRenderedTurnOffTexture();

  private:
    void requestRenderCaptureTurnOnTexture();
    void requestRenderCaptureTurnOffTexture();

  signals:

  public:
    KDDockWidgets::DockWidget* dock = nullptr;
    QLabel* labelTurnOnTexture = nullptr;
    QLabel* labelTurnOffTexture = nullptr;

  private:
    Qt3DRender::QFrameGraphNode* frameGraphParent = nullptr;
    Qt3DRender::QFrameGraphNode* frameGraph = nullptr;
    Qt3DRender::QLayerFilter* layerFilter = nullptr;

    Qt3DRender::QCamera* cameraEntityTurnOnTexture = nullptr;
    QVector3D cameraOffsetTurnOnTexture = QVector3D( 0, 0, 50 );

    TextureRenderTarget* textureTargetTurnOnTexture = nullptr;
    Qt3DRender::QRenderCapture* renderCaptureTurnOnTexture = nullptr;
    Qt3DRender::QRenderCaptureReply* replyTurnOnTexture = nullptr;

    Qt3DRender::QCamera* cameraEntityTurnOffTexture = nullptr;
    QVector3D cameraOffsetTurnOffTexture = QVector3D( 0, 0, 50 );

    TextureRenderTarget* textureTargetTurnOffTexture = nullptr;
    Qt3DRender::QRenderCapture* renderCaptureTurnOffTexture = nullptr;
    Qt3DRender::QRenderCaptureReply* replyTurnOffTexture = nullptr;

    QPointer<Implement> implement;

    std::vector<double> sectionOffsets;
    std::vector<uint16_t> sectionPixelOffsets;
};

class SectionControlFactory : public BlockFactory {
    Q_OBJECT

  public:
    SectionControlFactory( MyMainWindow* mainWindow,
                           KDDockWidgets::Location location,
                           QMenu* menu,
                           Qt3DCore::QEntity* rootEntity,
                           Qt3DRender::QFrameGraphNode* frameGraphParent )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ),
        rootEntity( rootEntity ),
        frameGraphParent( frameGraphParent ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Section Control" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Section Control" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      if( id != 0 && !isIdUnique( scene, id ) ) {
        id = QNEBlock::getNextUserId();
      }

      auto* object = new SectionControl( getNameOfFactory() + QString::number( id ),
                                         mainWindow,
                                         rootEntity,
                                         frameGraphParent );
      auto* b = createBaseBlock( scene, object, id );

      object->dock->setTitle( getNameOfFactory() );
      object->dock->setWidget( object->labelTurnOnTexture );

      menu->addAction( object->dock->toggleAction() );

      mainWindow->addDockWidget( object->dock, location );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Implement Data" ), QLatin1String( SLOT( setImplement( const QPointer<Implement> ) ) ) );
      b->addInputPort( QStringLiteral( "Section Control Data" ), QLatin1String( SLOT( setSections() ) ) );
      b->addInputPort( QStringLiteral( "Cultivated Area" ), QLatin1String( SLOT( setLayer( Qt3DRender::QLayer* ) ) ) );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
    Qt3DRender::QFrameGraphNode* frameGraphParent = nullptr;
};
