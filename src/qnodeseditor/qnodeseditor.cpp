/* Copyright (c) 2012, STANISLAW ADASZEWSKI
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in thes
      documentation and/or other materials provided with the distribution.
    * Neither the name of STANISLAW ADASZEWSKI nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "qnodeseditor.h"

#include <QObject>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QScrollBar>

#include <QDebug>

#include "qneport.h"
#include "qneconnection.h"
#include "qneblock.h"

QNodesEditor::QNodesEditor( QObject* parent ) :
  QObject( parent ) {
  conn = nullptr;
}

void QNodesEditor::install( QGraphicsScene* s ) {
  s->installEventFilter( this );
  scene = s;
}

QGraphicsItem* QNodesEditor::itemAt( const QPointF& pos ) {
  QList<QGraphicsItem*> items = scene->items( QRectF( pos - QPointF( 1, 1 ), QSize( 3, 3 ) ) );

  foreach( QGraphicsItem* item, items )
    if( item->type() > QGraphicsItem::UserType )
      return item;

  return nullptr;
}

bool QNodesEditor::eventFilter( QObject* o, QEvent* e ) {
  QGraphicsSceneMouseEvent* mouseEvent = ( QGraphicsSceneMouseEvent* ) e;
  QKeyEvent* keyEvent = ( QKeyEvent* ) e;

  switch( ( int ) e->type() ) {
    case QEvent::GraphicsSceneMousePress: {

        switch( ( int ) mouseEvent->button() ) {
          case Qt::LeftButton: {
              QGraphicsItem* item = itemAt( mouseEvent->scenePos() );

              if( item && item->type() == QNEPort::Type ) {

                foreach( QGraphicsView* it, scene->views() ) {
                  it->setDragMode( QGraphicsView::NoDrag );
                }

                conn = new QNEConnection( nullptr );
                scene->addItem( conn );
                conn->setPort1( ( QNEPort* ) item );
                conn->setPos1( item->scenePos() );
                conn->setPos2( mouseEvent->scenePos() );
                conn->updatePath();

                return true;
              }

              break;
            }

          case Qt::RightButton: {
              QGraphicsItem* item = itemAt( mouseEvent->scenePos() );

              if( !item ) {
                foreach( QGraphicsView* it, scene->views() ) {
                  it->setDragMode( QGraphicsView::ScrollHandDrag );
                }

                break;
              }

              QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

              if( block && block->deleteable ) {
                delete block;
                break;
              }

              QNEConnection* connection = qgraphicsitem_cast<QNEConnection*>( item );

              if( connection /*&& connection->deleteable*/ ) {
                delete connection;
                break;
              }
            }
            break;
        }

        break;

      case QEvent::KeyRelease: {
          if( keyEvent->matches( QKeySequence::Delete ) ) {
            foreach( QGraphicsItem* item, scene->selectedItems() ) {
              QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

              if( block && block->deleteable ) {
                delete block;
              }
            }
          }
        }
        break;
      }

      break;

    case QEvent::GraphicsSceneMouseMove: {
        QGraphicsSceneMouseEvent* m = static_cast<QGraphicsSceneMouseEvent*>( e );

        if( conn ) {
          conn->setPos2( mouseEvent->scenePos() );
          conn->updatePath();
          return true;
        } else {
          if( m->buttons() & Qt::RightButton ) {
            QPointF delta = m->lastScreenPos() - m->screenPos();

            foreach( QGraphicsView* view, scene->views() ) {
              double newX = view->horizontalScrollBar()->value() + delta.x();
              double newY = view->verticalScrollBar()->value() + delta.y();
              view->horizontalScrollBar()->setValue( int( newX ) );
              view->verticalScrollBar()->setValue( int( newY ) );
            }

            return true;
          }
        }

        break;
      }

    case QEvent::GraphicsSceneMouseRelease: {
        if( conn && mouseEvent->button() == Qt::LeftButton ) {
          QNEPort* port = qgraphicsitem_cast<QNEPort*>( itemAt( mouseEvent->scenePos() ) );

          if( port ) {
            if( conn->setPort2( port ) ) {
              conn->setPos2( port->scenePos() );
              conn->updatePath();
              conn = nullptr;
              return true;
            }
          }

          delete conn;
          conn = nullptr;
          return true;
        }

        foreach( QGraphicsView* it, scene->views() ) {
          it->setDragMode( QGraphicsView::RubberBandDrag );
        }

        break;
      }
  }

  return QObject::eventFilter( o, e );
}

void QNodesEditor::save( QDataStream& ds ) {
  foreach( QGraphicsItem* item, scene->items() )
    if( item->type() == QNEBlock::Type ) {
      ds << item->type();
      ( ( QNEBlock* ) item )->save( ds );
    }

  foreach( QGraphicsItem* item, scene->items() )
    if( item->type() == QNEConnection::Type ) {
      ds << item->type();
      ( ( QNEConnection* ) item )->save( ds );
    }
}

void QNodesEditor::load( QDataStream& ds ) {
  scene->clear();

  QMap<quint64, QNEPort*> portMap;

  while( !ds.atEnd() ) {
    int type;
    ds >> type;

    if( type == QNEBlock::Type ) {
      QNEBlock* block = new QNEBlock( nullptr );
      scene->addItem( block );
      block->load( ds, portMap );
    } else if( type == QNEConnection::Type ) {
      QNEConnection* conn = new QNEConnection( nullptr );
      scene->addItem( conn );
      conn->load( ds, portMap );
    }
  }
}
