// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include <QObject>
#include <QPaintedTextureImage>

class ArrowTexture : public Qt3DRender::QPaintedTextureImage {
  Q_OBJECT

public:
  ArrowTexture( Qt3DCore::QNode* parent = nullptr );

  void setSettings( int    arrowSize,
                    int    arrowWidth,
                    int    centerLineSize,
                    int    borderLineSize,
                    QColor arrowColor,
                    QColor centerLineColor,
                    QColor borderLineColor,
                    QColor backgroundColor );

  void setAnyDirectionArrows( bool anyDirection );

  void paint( QPainter* painter ) override;

private:
  int arrowSizePercent  = 20;
  int arrowWidthPercent = 90;
  int centerLinePercent = 5;
  int borderLinePercent = 0;

  bool anyDirection = false;

  QColor arrowColor      = QColor( 0xff, 0xff, 0, 200 );
  QColor centerLineColor = QColor( 0xf5, 0x00, 0x00, 255 );
  QColor borderLineColor = QColor( 0xff, 0xff, 0, 200 );
  QColor backgroundColor = QColor( 0xf5, 0x9f, 0xbd, 100 );
};
