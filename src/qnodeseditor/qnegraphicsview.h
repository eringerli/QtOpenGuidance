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

#include <QGraphicsView>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QtMath>
#include <QAbstractScrollArea>
#include <QDebug>

class QNEGraphicsView : public QGraphicsView {
    Q_OBJECT
  public:
    explicit QNEGraphicsView( QWidget* parent = nullptr )
      : QGraphicsView( parent ) {
      setDragMode( QGraphicsView::ScrollHandDrag );
    }

  public slots:
    void zoomIn() {
      scale( 1.1, 1.1 );
    }
    void zoomOut() {
      scale( 0.9, 0.9 );
    }

  protected:
    void wheelEvent( QWheelEvent* event ) override {

      // zoom
      const ViewportAnchor anchor = transformationAnchor();
      setTransformationAnchor( QGraphicsView::AnchorUnderMouse );
      int angle = event->angleDelta().y();
      qreal factor;

      if( angle > 0 ) {
        factor = 1.1;
      } else {
        factor = 0.9;
      }

      scale( factor, factor );
      setTransformationAnchor( anchor );
    }
};

//Q_DECLARE_METATYPE(QNEGraphicsView);

