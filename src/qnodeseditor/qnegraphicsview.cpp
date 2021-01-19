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

#include "qnegraphicsview.h"

#include <QWheelEvent>
#include <QKeyEvent>
#include <QAbstractScrollArea>

QNEGraphicsView::QNEGraphicsView( QWidget* parent )
  : QGraphicsView( parent ) {
  setDragMode( QGraphicsView::ScrollHandDrag );
}

void QNEGraphicsView::zoomIn() {
  scale( 1 + zoomFactor, 1 + zoomFactor );
}

void QNEGraphicsView::zoomOut() {
  scale( 1 - zoomFactor, 1 - zoomFactor );
}

void QNEGraphicsView::wheelEvent( QWheelEvent* event ) {
  // zoom
  const ViewportAnchor anchor = transformationAnchor();
  setTransformationAnchor( QGraphicsView::AnchorUnderMouse );
  int angle = event->angleDelta().y();
  qreal factor;

  if( angle > 0 ) {
    factor = 1 + zoomFactor;
  } else {
    factor = 1 - zoomFactor;
  }

  scale( factor, factor );
  setTransformationAnchor( anchor );
}
