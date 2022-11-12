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
