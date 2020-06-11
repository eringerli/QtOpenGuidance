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

#include "SectionControl.h"

#include <QtCore/QDebug>
#include <QtMath>

#include "../3d/texturerendertarget.h"
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QRenderCaptureReply>

SectionControl::SectionControl( const QString& uniqueName,
                                MyMainWindow* mainWindow,
                                Qt3DCore::QEntity* rootEntity,
                                Qt3DRender::QFrameGraphNode* frameGraphParent )
  : frameGraphParent( frameGraphParent ) {

  label = new QLabel( mainWindow );
  label->setScaledContents( true );

  dock = new KDDockWidgets::DockWidget( uniqueName );
  dock->setWidget( label );

  cameraEntity = new Qt3DRender::QCamera( rootEntity );
  cameraEntity->setProjectionType( Qt3DRender::QCameraLens::OrthographicProjection );
  cameraEntity->lens()->setOrthographicProjection( 10, -10, 10, -10, 0.1f, 30 );
  cameraEntity->setPosition( QVector3D( 0, 0, 20.0f ) );
  cameraEntity->setUpVector( QVector3D( 0, 1, 0 ) );
  cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );

  // Create a texture to render into. This acts as the buffer that
  // holds the rendered image.
  auto renderTargetSelector = new Qt3DRender::QRenderTargetSelector( frameGraphParent );
  auto textureTarget = new TextureRenderTarget( renderTargetSelector, QSize( 1, 400 ) );
  renderTargetSelector->setTarget( textureTarget );

  // Create a viewport node. The viewport here just covers the entire render area.
  auto viewport = new Qt3DRender::QViewport( renderTargetSelector );
  viewport->setNormalizedRect( QRectF( 0.0, 0.0, 1.0, 1.0 ) );

  // Create a camera selector node, and tell it to use the camera we've ben given.
  auto cameraSelector = new Qt3DRender::QCameraSelector( viewport );
  cameraSelector->setCamera( cameraEntity );

  // Create a node used for clearing the required buffers.
  auto clearBuffers = new Qt3DRender::QClearBuffers( cameraSelector );
  clearBuffers->setClearColor( QColor( 0, 0, 0, 255 ) );
  clearBuffers->setBuffers( Qt3DRender::QClearBuffers::ColorDepthBuffer );

  layerFilter = new Qt3DRender::QLayerFilter( clearBuffers );
  layerFilter->setFilterMode( Qt3DRender::QLayerFilter::AcceptAnyMatchingLayers );

  auto frustumCulling = new Qt3DRender::QFrustumCulling( layerFilter );

  renderCapture = new Qt3DRender::QRenderCapture( frustumCulling );

  frameGraph = renderTargetSelector;

  requestRenderCapture();
}

// order is important! Crashes if a parent entity is removed first!
SectionControl::~SectionControl() {
  frameGraph->deleteLater();
  reply->deleteLater();
  label->deleteLater();
  dock->deleteLater();
}

void SectionControl::setPose( const Point_3 position, const QQuaternion orientation, const PoseOption::Options options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    if( implement != nullptr ) {
      auto transform = cameraEntity->transform();

      transform->setTranslation( convertPoint3ToQVector3D( position ) + QVector3D( 0, 0, 20 ) );
      transform->setRotation( orientation );
    }
  }
}

void SectionControl::setImplement( const QPointer<Implement>& implement ) {
  if( implement != nullptr ) {
    this->implement = implement;
    size_t numSections = implement->sections.size();

    // get the left most point of the implement
    double sectionOffset = 0;

    for( size_t i = 0; i < numSections; ++i ) {
      const auto section = implement->sections.at( i );
      sectionOffset += section->widthOfSection - section->overlapLeft - section->overlapRight;
    }

    sectionOffset /= 2;

    sectionOffsets.clear();
    sectionOffsets.reserve( ( numSections - 1 ) * 2 );

    for( size_t i = 1; i < numSections; ++i ) {
      const auto section = implement->sections.at( i );
      sectionOffset += section->overlapLeft;
      sectionOffsets.push_back( sectionOffset );
      sectionOffset -= section->widthOfSection;
      sectionOffsets.push_back( sectionOffset );
      sectionOffset += section->overlapRight;
    }

    cameraEntity->lens()->setOrthographicProjection( 0.1f, 0, float( sectionOffsets.front() ), float( sectionOffsets.back() ),  0.1f, 1000 );

  }
}

void SectionControl::setSections() {
  if( implement != nullptr ) {
//    size_t numSections = implement->sections.size();

//    const auto& section0 = implement->sections.at( 0 );
//    const auto& state0 = section0->state();
//    const bool globalForceOff = state0.testFlag( ImplementSection::State::ForceOff );
//    const bool globalForceOn = state0.testFlag( ImplementSection::State::ForceOn );

//    for( size_t i = 1; i < numSections; ++i ) {
//      size_t sectionIndex = i - 1;
//      const auto& section = implement->sections.at( i );
//      const auto& state = section->state();

//      if( !globalForceOff &&
//          ( !state.testFlag( ImplementSection::State::ForceOff ) ) &&
//          ( globalForceOn || section->isSectionOn() ) ) {
//        if( sectionMeshes.at( sectionIndex ) == nullptr ) {
//          sectionMeshes.at( sectionIndex ) = createNewMesh();
//        }
//      } else {
//        if( sectionMeshes.at( sectionIndex ) != nullptr ) {
//          sectionMeshes.at( sectionIndex )->optimise( threadForCgalWorker );
//          sectionMeshes.at( sectionIndex ) = nullptr;
//        }
//      }
//    }
  }
}

void SectionControl::setLayer( Qt3DRender::QLayer* layer ) {
  layerFilter->addLayer( layer );
}

void SectionControl::onImageRendered() {
  // Get the image from the reply and display it in the label.
  label->setPixmap( QPixmap::fromImage( reply->image() ) );

  // Request another render capture.
  requestRenderCapture();
}

void SectionControl::requestRenderCapture() {
  if( reply != nullptr ) {
    reply->deleteLater();
  }

  reply = renderCapture->requestCapture();
  QObject::connect( reply, &Qt3DRender::QRenderCaptureReply::completed, this, &SectionControl::onImageRendered );
}
