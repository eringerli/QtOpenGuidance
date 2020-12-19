/* Copyright (c) 2012, STANISLAW ADASZEWSKI
 * Copyright (c) 2020, Christian Riggenbach
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of STANISLAW ADASZEWSKI nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <QObject>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QScrollBar>

class QNEGestureEventFilter : public QObject {
    Q_OBJECT

  public:
    QNEGestureEventFilter( QObject* parent = nullptr )
      : QObject( parent ) {
      parent->installEventFilter( this );
    }

  protected:
    bool eventFilter( QObject* object, QEvent* event ) override {

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

};

