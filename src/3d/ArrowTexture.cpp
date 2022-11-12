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

#include "ArrowTexture.h"

#include <QPainter>
#include <QPainterPath>
#include <QSize>

ArrowTexture::ArrowTexture( Qt3DCore::QNode* parent ) : QPaintedTextureImage( parent ) { setSize( QSize( 512, 512 ) ); }

void
ArrowTexture::setSettings( int    arrowSize,
                           int    arrowWidth,
                           int    centerLineSize,
                           int    borderLineSize,
                           QColor arrowColor,
                           QColor centerLineColor,
                           QColor borderLineColor,
                           QColor backgroundColor ) {
  this->arrowSizePercent  = arrowSize;
  this->arrowWidthPercent = arrowWidth;
  this->centerLinePercent = centerLineSize;
  this->borderLinePercent = borderLineSize;

  this->arrowColor      = arrowColor;
  this->centerLineColor = centerLineColor;
  this->borderLineColor = borderLineColor;
  this->backgroundColor = backgroundColor;

  update();
}

void
ArrowTexture::setAnyDirectionArrows( bool anyDirection ) {
  if( this->anyDirection != anyDirection ) {
    this->anyDirection = anyDirection;
    update();
  }
}

void
ArrowTexture::paint( QPainter* painter ) {
  painter->setCompositionMode( QPainter::CompositionMode_Source );

  painter->setBackground( backgroundColor );
  painter->eraseRect( QRect( 0, 0, width(), height() ) );

  painter->setCompositionMode( QPainter::CompositionMode_Source );

  painter->setBrush( arrowColor );
  painter->setPen( Qt::NoPen );

  if( arrowSizePercent ) {
    qreal arrowWidth = width() * ( qreal( 100 - arrowSizePercent ) / 100 );

    if( anyDirection ) {
      const QPointF points[4] = { QPointF( width(), height() / 2 ),
                                  QPointF( ( width() + arrowWidth ) / 2,
                                           qreal( height() * arrowWidthPercent / 100.0f ) / 2 + height() / 2 ),
                                  QPointF( arrowWidth, height() / 2 ),
                                  QPointF( ( width() + arrowWidth ) / 2, qreal( height() * ( 100 - arrowWidthPercent ) / 100.0f ) / 2 ) };
      painter->drawConvexPolygon( points, 4 );
    } else {
      const QPointF points[4] = { QPointF( width(), height() / 2 ),
                                  QPointF( arrowWidth / 2, qreal( height() * arrowWidthPercent / 100.0f ) / 2 + height() / 2 ),
                                  QPointF( arrowWidth, height() / 2 ),
                                  QPointF( arrowWidth / 2, qreal( height() * ( 100 - arrowWidthPercent ) / 100.0f ) / 2 ) };
      painter->drawConvexPolygon( points, 4 );
    }
  }

  if( centerLinePercent ) {
    painter->setBrush( centerLineColor );
    painter->drawRect(
      QRectF( 0, ( ( 50 - ( qreal( centerLinePercent ) / 2 ) ) / 100 ) * height(), width(), qreal( centerLinePercent ) / 100 * height() ) );
  }

  if( borderLinePercent ) {
    painter->setBrush( borderLineColor );
    painter->drawRect( QRectF( 0, 0, width(), ( ( qreal( borderLinePercent ) / 2 ) / 100 ) * height() ) );
    painter->drawRect( QRectF( 0, height(), width(), -( ( qreal( borderLinePercent ) / 2 ) / 100 ) * height() ) );
  }
}
