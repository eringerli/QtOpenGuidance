// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef ARROWTEXTURE_H
#define ARROWTEXTURE_H

#include <QObject>
#include <QPaintedTextureImage>
#include <QPainter>
#include <QPainterPath>
#include <QColor>
#include <QSize>

#include <QDebug>

class ArrowTexture : public Qt3DRender::QPaintedTextureImage {
    Q_OBJECT

  public:
    ArrowTexture( Qt3DCore::QNode* parent = nullptr )
      : QPaintedTextureImage( parent ) {
      setSize( QSize( 500, 500 ) );
    }

    void setSettings( QColor arrowColor, QColor backgroundcolor,
                      float arrowSize, float distanceBetweenArrows ) {
      this->arrowColor = arrowColor;
      this->backgroundColor = backgroundcolor;
      this->arrowSize = arrowSize;
      this->distanceBetweenArrows = distanceBetweenArrows;

      update();
    }

    void paint( QPainter* painter ) override {
      painter->setCompositionMode( QPainter::CompositionMode_Source );

      painter->setBackground( backgroundColor );
      painter->eraseRect( QRect( 0, 0, width(), height() ) );

      painter->setCompositionMode( QPainter::CompositionMode_SourceOver );

      painter->setBrush( arrowColor );
      painter->setPen( Qt::NoPen );

      qreal pixelPerMeter = width() / qreal( arrowSize + distanceBetweenArrows );

      const QPointF points[4] = {
        QPointF( width(), height() / 2 ),
        QPointF( width() - ( qreal( arrowSize ) * 2 * pixelPerMeter ), height() ),
        QPointF( width() - ( qreal( arrowSize )*pixelPerMeter ), height() / 2 ),
        QPointF( width() - ( qreal( arrowSize ) * 2 * pixelPerMeter ), 0 )
      };
      painter->drawConvexPolygon( points, 4 );

    }

  private:
    float arrowSize = 2;
    float distanceBetweenArrows = 3;
    QColor arrowColor = QColor( 0xff, 0xff, 0, 200 );
    QColor backgroundColor = QColor( 0xf5, 0x9f, 0xbd, 100 );
};

#endif // ARROWTEXTURE_H
