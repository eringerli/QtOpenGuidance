// Copyright: see here https://github.com/florianblume/Qt3D-OffscreenRenderer

#pragma once

#include <QObject>
#include <QSize>

#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>

#include "3d/qt3dForwards.h"

// Encapsulates a 2D texture that a frame graph can render into.
class TextureRenderTarget : public Qt3DRender::QRenderTarget {
  Q_OBJECT
public:
  TextureRenderTarget( Qt3DCore::QNode*                                 parent           = nullptr,
                       QSize                                            size             = QSize( 500, 500 ),
                       Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint = Qt3DRender::QRenderTargetOutput::Color0 );

  void                    setSize( QSize size );
  QSize                   getSize();
  Qt3DRender::QTexture2D* getTexture();

private:
  QSize                            size;
  Qt3DRender::QRenderTargetOutput* output;
  Qt3DRender::QTexture2D*          texture;
  // To enable depth testing
  Qt3DRender::QRenderTargetOutput* depthTextureOutput;
  Qt3DRender::QTexture2D*          depthTexture;
};
