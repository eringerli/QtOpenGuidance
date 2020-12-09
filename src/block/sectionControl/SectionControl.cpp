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
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QRenderCaptureReply>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QViewport>

#include "../helpers/eigenHelper.h"

SectionControl::SectionControl( const QString& uniqueName,
                                MyMainWindow* mainWindow,
                                Qt3DCore::QEntity* rootEntity,
                                Qt3DRender::QFrameGraphNode* frameGraphParent )
  : frameGraphParent( frameGraphParent ) {

  auto* layout = new QVBoxLayout;

  labelTurnOnTexture = new QLabel();
  labelTurnOnTexture->setScaledContents( true );
  layout->addWidget( labelTurnOnTexture );

  labelTurnOffTexture = new QLabel();
  labelTurnOffTexture->setScaledContents( true );
  layout->addWidget( labelTurnOffTexture );

  auto* widget = new QWidget( mainWindow );
  widget->setLayout( layout );

  dock = new KDDockWidgets::DockWidget( uniqueName );
  dock->setWidget( widget );


  // Create a viewport node. The viewport here just covers the entire render area.
  auto* viewport = new Qt3DRender::QViewport( frameGraphParent );
  viewport->setNormalizedRect( QRectF( 0.0, 0.0, 1.0, 1.0 ) );

  // Create a node used for clearing the required buffers.
  auto* clearBuffers = new Qt3DRender::QClearBuffers( viewport );
  clearBuffers->setClearColor( QColor( 0, 0, 0, 255 ) );
  clearBuffers->setBuffers( Qt3DRender::QClearBuffers::ColorDepthBuffer );

  auto* frustumCulling = new Qt3DRender::QFrustumCulling( clearBuffers );

  layerFilter = new Qt3DRender::QLayerFilter( frustumCulling );
  layerFilter->setFilterMode( Qt3DRender::QLayerFilter::AcceptAnyMatchingLayers );

  {
    // Create a texture to render into. This acts as the buffer that
    // holds the rendered image.
    auto* renderTargetSelector = new Qt3DRender::QRenderTargetSelector( layerFilter );
    textureTargetTurnOnTexture = new TextureRenderTarget( renderTargetSelector, QSize( 400, 1 ) );
    renderTargetSelector->setTarget( textureTargetTurnOnTexture );

    cameraEntityTurnOnTexture = new Qt3DRender::QCamera( rootEntity );
    cameraEntityTurnOnTexture->setProjectionType( Qt3DRender::QCameraLens::OrthographicProjection );
    cameraEntityTurnOnTexture->lens()->setOrthographicProjection( 10, -10, 10, -10, 0.1f, 30 );
    cameraEntityTurnOnTexture->setPosition( QVector3D( 0, 0, 20.0f ) );
    cameraEntityTurnOnTexture->setUpVector( QVector3D( 0, 1, 0 ) );
    cameraEntityTurnOnTexture->setViewCenter( QVector3D( 0, 0, 0 ) );

    // Create a camera selector node, and tell it to use the camera we've ben given.
    auto* cameraSelectorTurnOnTexture = new Qt3DRender::QCameraSelector( textureTargetTurnOnTexture );
    cameraSelectorTurnOnTexture->setCamera( cameraEntityTurnOnTexture );

    renderCaptureTurnOnTexture = new Qt3DRender::QRenderCapture( cameraSelectorTurnOnTexture );
  }

  {
    // Create a texture to render into. This acts as the buffer that
    // holds the rendered image.
    auto* renderTargetSelector = new Qt3DRender::QRenderTargetSelector( layerFilter );
    textureTargetTurnOffTexture = new TextureRenderTarget( renderTargetSelector, QSize( 400, 1 ) );
    renderTargetSelector->setTarget( textureTargetTurnOffTexture );

    cameraEntityTurnOffTexture = new Qt3DRender::QCamera( rootEntity );
    cameraEntityTurnOffTexture->setProjectionType( Qt3DRender::QCameraLens::OrthographicProjection );
    cameraEntityTurnOffTexture->lens()->setOrthographicProjection( 10, -10, 10, -10, 0.1f, 30 );
    cameraEntityTurnOffTexture->setPosition( QVector3D( 0, 0, 20.0f ) );
    cameraEntityTurnOffTexture->setUpVector( QVector3D( 0, 1, 0 ) );
    cameraEntityTurnOffTexture->setViewCenter( QVector3D( 0, 0, 0 ) );

    // Create a camera selector node, and tell it to use the camera we've ben given.
    auto* cameraSelectorTurnOffTexture = new Qt3DRender::QCameraSelector( textureTargetTurnOffTexture );
    cameraSelectorTurnOffTexture->setCamera( cameraEntityTurnOffTexture );

    renderCaptureTurnOffTexture = new Qt3DRender::QRenderCapture( cameraSelectorTurnOffTexture );
  }

  frameGraph = viewport;

  requestRenderCaptureTurnOnTexture();
}

// order is important! Crashes if a parent entity is removed first!
SectionControl::~SectionControl() {
  frameGraph->deleteLater();
  replyTurnOnTexture->deleteLater();
  labelTurnOnTexture->deleteLater();
  dock->deleteLater();
}

void SectionControl::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    if( implement != nullptr ) {
      auto* transformOn = cameraEntityTurnOnTexture->transform();

      auto qqauternion = toQQuaternion( orientation );
      transformOn->setTranslation( toQVector3D( position ) + qqauternion * QVector3D( 1.5f, 0, 20 ) );
      transformOn->setRotation( qqauternion * QQuaternion::fromAxisAndAngle( 0, 0, 1, 90 ) );

      auto* transformOff = cameraEntityTurnOffTexture->transform();

      transformOff->setTranslation( toQVector3D( position ) + qqauternion * QVector3D( 0.5f, 0, 20 ) );
      transformOff->setRotation( qqauternion * QQuaternion::fromAxisAndAngle( 0, 0, 1, 90 ) );

      requestRenderCaptureTurnOnTexture();
      requestRenderCaptureTurnOffTexture();
    }
  }
}

void SectionControl::setImplement( const QPointer<Implement>& implement ) {
  if( implement != nullptr ) {
    this->implement = implement;
    size_t numSections = implement->sections.size();

    // get the left most point of the implement
    double firstSectionOffset = 0;

    for( size_t i = 0; i < numSections; ++i ) {
      auto* const section = implement->sections.at( i );
      firstSectionOffset += section->widthOfSection - section->overlapLeft - section->overlapRight;
    }

    firstSectionOffset /= 2;
    double sectionOffset = firstSectionOffset;

    sectionOffsets.clear();
    sectionOffsets.reserve( ( numSections - 1 ) * 2 );

    double totalWidthOfSections = 0;

    for( size_t i = 1; i < numSections; ++i ) {
      auto* const section = implement->sections.at( i );
      sectionOffset += section->overlapLeft;
      sectionOffsets.push_back( sectionOffset );
      sectionOffset -= section->widthOfSection;
      totalWidthOfSections += section->widthOfSection;
      sectionOffsets.push_back( sectionOffset );
      sectionOffset += section->overlapRight;
    }

    double totalWidth = sectionOffsets.front() - sectionOffsets.back();
//    double totalSpace = 0;


    qDebug() << "SectionControl::setImplement" << sectionOffsets.front() << sectionOffsets.back() << totalWidthOfSections << totalWidth << totalWidthOfSections / totalWidth << totalWidth / totalWidthOfSections;

    int widthOfTexture = std::ceil( totalWidth / totalWidthOfSections * double( ( numSections - 1 ) * 10 ) );
    textureTargetTurnOnTexture->setSize( QSize( widthOfTexture, 1 ) );
    textureTargetTurnOffTexture->setSize( QSize( widthOfTexture, 1 ) );
    qDebug() << "SectionControl::setImplement" << textureTargetTurnOnTexture->getSize();

    cameraEntityTurnOnTexture->lens()->setOrthographicProjection( float( sectionOffsets.front() ), float( sectionOffsets.back() ), 0, -0.1f, 0.1f, 100 );
    cameraEntityTurnOffTexture->lens()->setOrthographicProjection( float( sectionOffsets.front() ), float( sectionOffsets.back() ), 0, -0.1f, 0.1f, 100 );


    sectionPixelOffsets.clear();
    sectionPixelOffsets.reserve( numSections - 1 );
    double pixelPerMeter = widthOfTexture / totalWidth;

    for( size_t i = 0, end = sectionOffsets.size(); i < end; i += 2 ) {
      sectionPixelOffsets.push_back( -( sectionOffsets.at( i ) - firstSectionOffset )*pixelPerMeter );
      qDebug() << "sectionPixelOffsets" << pixelPerMeter << i << ( sectionOffsets.at( i ) - firstSectionOffset ) << sectionPixelOffsets.back();
    }

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

void SectionControl::onImageRenderedTurnOnTexture() {
  // Get the image from the reply and display it in the label.
  labelTurnOnTexture->setPixmap( QPixmap::fromImage( replyTurnOnTexture->image() ) );

  auto* rgbData = ( QRgb* )replyTurnOnTexture->image().constBits();

  constexpr uint8_t numPixelOnToTurnOn = 5;

  bool emitSectionsChanged = false;

  for( size_t i = 0, end = sectionPixelOffsets.size(); i < end; ++i ) {
    if( implement->sections.at( i + 1 )->state().testFlag( ImplementSection::Automatic ) ) {
      const auto sectionPixelOffset = sectionPixelOffsets.at( i );
      uint16_t sectionPixelMax = sectionPixelOffset + 10;

      if( sectionPixelMax > replyTurnOnTexture->image().width() ) {
        qDebug() << "sectionPixelOffset<(reply->image().width())" << sectionPixelMax << ( replyTurnOnTexture->image().width() - 1 );
        sectionPixelMax = replyTurnOnTexture->image().width();
      }

      uint8_t numPixelOn = 0;

      for( uint16_t i = sectionPixelOffset; i < sectionPixelMax; ++i ) {
        if( ( rgbData[i] & 0x00ffffff ) != 0 ) {
          ++numPixelOn;
        }
      }

      if( numPixelOn >= numPixelOnToTurnOn ) {
        implement->sections.at( i + 1 )->removeState( ImplementSection::AutomaticOn );
        qDebug() << "Automatic On 1:  " << i << implement->sections.at( i + 1 )->state();
        emitSectionsChanged = true;
      } else {
        implement->sections.at( i + 1 )->addState( ImplementSection::AutomaticOn );
        qDebug() << "Automatic On 2: " << i << implement->sections.at( i + 1 )->state();
        emitSectionsChanged = true;
      }
    }
  }

  if( emitSectionsChanged ) {
    implement->emitSectionsChanged();
  }

  //  requestRenderCapture();
}

void SectionControl::onImageRenderedTurnOffTexture() {
  // Get the image from the reply and display it in the label.
  labelTurnOffTexture->setPixmap( QPixmap::fromImage( replyTurnOffTexture->image() ) );

  auto* rgbData = ( QRgb* )replyTurnOffTexture->image().constBits();

  constexpr uint8_t numPixelOnToTurnOff = 5;

  bool emitSectionsChanged = false;

  for( size_t i = 0, end = sectionPixelOffsets.size(); i < end; ++i ) {
    if( implement->sections.at( i + 1 )->state().testFlag( ImplementSection::Automatic ) ) {
      const auto sectionPixelOffset = sectionPixelOffsets.at( i );
      uint16_t sectionPixelMax = sectionPixelOffset + 10;

      if( sectionPixelMax > replyTurnOffTexture->image().width() ) {
        qDebug() << "sectionPixelOffset<(reply->image().width())" << sectionPixelMax << ( replyTurnOffTexture->image().width() - 1 );
        sectionPixelMax = replyTurnOffTexture->image().width();
      }

      uint8_t numPixelOn = 0;

      for( uint16_t i = sectionPixelOffset; i < sectionPixelMax; ++i ) {
        if( ( rgbData[i] & 0x00ffffff ) != 0 ) {
          ++numPixelOn;
        }
      }

      if( numPixelOn >= numPixelOnToTurnOff ) {
        implement->sections.at( i + 1 )->addState( ImplementSection::AutomaticOff );
        qDebug() << "Automatic Off 1: " << i << implement->sections.at( i + 1 )->state();
        emitSectionsChanged = true;
      } else {
        implement->sections.at( i + 1 )->removeState( ImplementSection::AutomaticOff );
        qDebug() << "Automatic Off 2: " << i << implement->sections.at( i + 1 )->state();
        emitSectionsChanged = true;
      }
    }
  }

  if( emitSectionsChanged ) {
    implement->emitSectionsChanged();
  }
}

void SectionControl::requestRenderCaptureTurnOnTexture() {
  if( replyTurnOnTexture != nullptr ) {
    replyTurnOnTexture->deleteLater();
  }

  replyTurnOnTexture = renderCaptureTurnOnTexture->requestCapture();
  QObject::connect( replyTurnOnTexture, &Qt3DRender::QRenderCaptureReply::completed, this, &SectionControl::onImageRenderedTurnOnTexture );
}

void SectionControl::requestRenderCaptureTurnOffTexture() {
  if( replyTurnOffTexture != nullptr ) {
    replyTurnOffTexture->deleteLater();
  }

  replyTurnOffTexture = renderCaptureTurnOffTexture->requestCapture();
  QObject::connect( replyTurnOffTexture, &Qt3DRender::QRenderCaptureReply::completed, this, &SectionControl::onImageRenderedTurnOffTexture );
}
