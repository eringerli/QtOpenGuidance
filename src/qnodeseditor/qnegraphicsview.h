#ifndef QNEGRAPHICSVIEW_H
#define QNEGRAPHICSVIEW_H

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
    void wheelEvent( QWheelEvent* event ) {
//          if (event->modifiers() & Qt::ControlModifier) {
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
//                  } else {
//                      QGraphicsView::wheelEvent(event);
//                  }

//          qreal deltaScale = 1;
//           deltaScale += event->delta() > 0 ? 0.1 : -0.1;
//           setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
//           scale(deltaScale, deltaScale);
    }
};

//Q_DECLARE_METATYPE(QNEGraphicsView);

#endif // QNEGRAPHICSVIEW_H
