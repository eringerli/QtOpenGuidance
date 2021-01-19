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

#include "qnegestures.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QScrollBar>

QNEGestureEventFilter::QNEGestureEventFilter( QObject* parent )
  : QObject( parent ) {
  parent->installEventFilter( this );
}

bool QNEGestureEventFilter::eventFilter( QObject* object, QEvent* event ) {

  QGraphicsView* view = qobject_cast<QGraphicsView*>( object );

  if( view ) {

    static QTransform originalTransform = QTransform();
    static int originalScrollbarHorizontal = 0;
    static int originalScrollbarVertical = 0;

    if( event->type() == QEvent::TouchBegin ) {
      QTouchEvent* touchEvent = static_cast<QTouchEvent*>( event );

      if( touchEvent->touchPoints().count() > 1 ) {
        //            qDebug() << QStringLiteral( "QEvent::TouchBegin" );
        return true;
      }
    }

    if( event->type() == QEvent::Gesture ) {
      QGestureEvent* gestureEvent = static_cast<QGestureEvent*>( event );

      //          if( QGesture* swipe = gestureEvent->gesture( Qt::SwipeGesture ) ) {
      //            qDebug() << "swipe" << swipe;
      //          }

      //          if( QGesture* pan = gestureEvent->gesture( Qt::PanGesture ) ) {
      //            qDebug() << "pan" << pan;
      //          }

      if( QPinchGesture* pinch = static_cast<QPinchGesture*>( gestureEvent->gesture( Qt::PinchGesture ) ) ) {
        if( pinch->state() == Qt::GestureStarted ) {
          originalTransform = view->transform();
          originalScrollbarHorizontal = view->horizontalScrollBar()->value();
          originalScrollbarVertical = view->verticalScrollBar()->value();
        }

        QTransform currentTransform = originalTransform;
        currentTransform.scale( pinch->totalScaleFactor(), pinch->totalScaleFactor() );
        view->setTransform( currentTransform );

        QLineF panLine( pinch->startCenterPoint(), pinch->centerPoint() );
        view->horizontalScrollBar()->setValue( originalScrollbarHorizontal - int( panLine.dx() ) );
        view->verticalScrollBar()->setValue( originalScrollbarVertical - int( panLine.dy() ) );

        event->accept();
      }

      return true;
    }

    //        switch( event->type() ) {
    //          case QEvent::TouchBegin: {
    //            qDebug() << "QEvent::TouchBegin";
    //            originalTransform = view->transform();
    //            originalScrollbarHorizontal = view->horizontalScrollBar()->value();
    //                originalScrollbarVertical = view->verticalScrollBar()->value();
    //            QTouchEvent* touchEvent = static_cast<QTouchEvent*>( event );
    //            touchEvent->accept();
    //          }

    //          case QEvent::TouchUpdate:
    //          case QEvent::TouchEnd: {
    //            QTouchEvent* touchEvent = static_cast<QTouchEvent*>( event );
    //            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

    //            if( touchPoints.count() >= 2 ) {
    //              // determine scale factor
    //              const QTouchEvent::TouchPoint& touchPoint0 = touchPoints.first();
    //              const QTouchEvent::TouchPoint& touchPoint1 = touchPoints.last();

    //              QLineF currentScaleLine = QLineF( touchPoint0.pos(), touchPoint1.pos() );
    //              QLineF originalScaleLine = QLineF( touchPoint0.startPos(), touchPoint1.startPos() );

    //              qreal currentScaleFactor = currentScaleLine.length() / originalScaleLine.length();
    //              QLineF panLine( originalScaleLine.center(), currentScaleLine.center() );

    //              qDebug() << currentScaleFactor << panLine << panLine.dx() << panLine.dy();

    //              QTransform currentTransform = originalTransform;

    //              currentTransform.scale( currentScaleFactor, currentScaleFactor );

    //              qDebug() << currentTransform;

    //              view->setTransform( currentTransform );

    //              view->horizontalScrollBar()->setValue( originalScrollbarHorizontal - int(panLine.dx()) );
    //              view->verticalScrollBar()->setValue( originalScrollbarVertical - int(panLine.dy()) );
    //            }

    //            return true;
    //          }
    //          break;
    //
    //          default:
    //            break;
    //        }

  }

  return false;
}
