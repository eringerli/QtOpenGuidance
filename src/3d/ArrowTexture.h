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
      setSize( QSize( 512, 512 ) );
    }

    void setSettings( int arrowSize, int arrowWidth, int centerLineSize, int borderLineSize,
                      QColor arrowColor, QColor centerLineColor, QColor borderLineColor, QColor backgroundColor ) {

      this->arrowSizePercent = arrowSize;
      this->arrowWidthPercent = arrowWidth;
      this->centerLinePercent = centerLineSize;
      this->borderLinePercent = borderLineSize;

      this->arrowColor = arrowColor;
      this->centerLineColor = centerLineColor;
      this->borderLineColor = borderLineColor;
      this->backgroundColor = backgroundColor;

      update();
    }

    void setAnyDirectionArrows( bool anyDirection ) {
      if( this->anyDirection != anyDirection ) {
        this->anyDirection = anyDirection;
        update();
      }
    }

    void paint( QPainter* painter ) override {
      painter->setCompositionMode( QPainter::CompositionMode_Source );

      painter->setBackground( backgroundColor );
      painter->eraseRect( QRect( 0, 0, width(), height() ) );

      painter->setCompositionMode( QPainter::CompositionMode_Source );

      painter->setBrush( arrowColor );
      painter->setPen( Qt::NoPen );


      if( arrowSizePercent ) {
        qreal arrowWidth =  width() * ( qreal( 100 - arrowSizePercent ) / 100 );

        if( anyDirection ) {
          const QPointF points[4] = {
            QPointF( width(), height() / 2 ),
            QPointF( ( width() + arrowWidth ) / 2, qreal( height()*arrowWidthPercent / 100.0f ) / 2 + height() / 2 ),
            QPointF( arrowWidth, height() / 2 ),
            QPointF( ( width() + arrowWidth ) / 2, qreal( height() * ( 100 - arrowWidthPercent ) / 100.0f ) / 2 )
          };
          painter->drawConvexPolygon( points, 4 );
        } else {
          const QPointF points[4] = {
            QPointF( width(), height() / 2 ),
            QPointF( arrowWidth / 2, qreal( height()*arrowWidthPercent / 100.0f ) / 2 + height() / 2 ),
            QPointF( arrowWidth, height() / 2 ),
            QPointF( arrowWidth / 2, qreal( height() * ( 100 - arrowWidthPercent ) / 100.0f ) / 2 )
          };
          painter->drawConvexPolygon( points, 4 );
        }
      }

      if( centerLinePercent ) {
        painter->setBrush( centerLineColor );
        painter->drawRect( QRectF(
                                   0, ( ( 50 - ( qreal( centerLinePercent ) / 2 ) ) / 100 )* height(),
                                   width(), qreal( centerLinePercent ) / 100 * height() ) );
      }

      if( borderLinePercent ) {
        painter->setBrush( borderLineColor );
        painter->drawRect( QRectF(
                                   0,       0,
                                   width(), ( ( qreal( borderLinePercent ) / 2 ) / 100 )* height() ) );
        painter->drawRect( QRectF(
                                   0,       height(),
                                   width(), -( ( qreal( borderLinePercent ) / 2 ) / 100 )* height() ) );
      }


//      qDebug()<<0<< int( ( width() / 2 ) - qreal( centerLine/200 )* width() )<<
//          int( height() )<< int( ( width() / 2 ) + qreal( centerLine/200 )* width() );
//      painter->drawRect(int((height() / 2)-qreal(centerLine)* pixelPerMeter/2),0,
//                        int(qreal(centerLine)* pixelPerMeter),width());
    }

  private:
    int arrowSizePercent = 20;
    int arrowWidthPercent = 90;
    int centerLinePercent = 5;
    int borderLinePercent = 0;

    bool anyDirection = false;

    QColor arrowColor = QColor( 0xff, 0xff, 0, 200 );
    QColor centerLineColor = QColor( 0xf5, 0x00, 0x00, 255 );
    QColor borderLineColor = QColor( 0xff, 0xff, 0, 200 );
    QColor backgroundColor = QColor( 0xf5, 0x9f, 0xbd, 100 );
};

#endif // ARROWTEXTURE_H
