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

#include "qneport.h"
#include "block/BlockBase.h"
#include "qgraphicsitem.h"
#include "qnamespace.h"
#include "qneblock.h"

#include <QFontMetrics>
#include <QTextDocument>

#include <QGraphicsScene>

#include <QPen>

#include <QEvent>
#include <QGraphicsSceneMouseEvent>

#include <utility>

#include "qneconnection.h"

QNEPort::QNEPort( const BlockPort* blockPort, QGraphicsItem* parent ) : QGraphicsPathItem( parent ), blockPort( blockPort ) {
  label = new QGraphicsTextItem( this );
  label->setDefaultTextColor( Qt::black );
  setName( blockPort->name );

  flags.setFlag( Flag::Output, blockPort->isOutput() );
  flags.setFlag( Flag::SquareBullet, blockPort->isSquareBullet() );

  // QPainterPath p;
  // p.addEllipse( -radiusOfBullet, -radiusOfBullet, 2 * radiusOfBullet, 2 * radiusOfBullet );
  // setPath( p );

  // if( embedded ) {
  //   setBrush( QColor( "plum" ) );
  //   setPen( QPen( QColor( "purple" ) ) );
  // } else {
  setBrush( Qt::red );
  setPen( QPen( Qt::darkRed ) );
  // }

  setFlag( QGraphicsItem::ItemSendsScenePositionChanges );

  applyPortFlags();
}

QNEPort::QNEPort( const QString& name, QGraphicsItem* parent ) : QGraphicsPathItem( parent ) {
  label = new QGraphicsTextItem( this );
  label->setDefaultTextColor( Qt::black );
  setName( name );

  flags.setFlag( Flag::Output, false );

  setFlag( QGraphicsItem::ItemSendsScenePositionChanges );

  applyPortFlags();
}

QNEPort::~QNEPort() {
  if( porthelper != nullptr ) {
    porthelper->deleteLater();
  }
}

void
QNEPort::setName( const QString& n ) const {
  label->setPlainText( n );
}

QString
QNEPort::getName() const {
  return label->toPlainText();
}

void
QNEPort::recalculateLabelPosition() {
  if( isOutput() ) {
    label->setPos( -( label->boundingRect().width() + radiusOfBullet ), -label->boundingRect().height() / 2. );
  } else {
    label->setPos( radiusOfBullet, -label->boundingRect().height() / 2. );
  }
}

void
QNEPort::applyPortFlags() {
  if( isEmbedded() ) {
    setBrush( QColor( "plum" ) );
    setPen( QPen( QColor( "purple" ) ) );
  } else {
    setBrush( Qt::red );
    setPen( QPen( Qt::darkRed ) );
  }

  if( isTypePort() ) {
    QFont font( scene()->font() );
    font.setItalic( true );
    label->setFont( font );
  } else {
    if( isNamePort() ) {
      QFont font( scene()->font() );
      font.setBold( true );
      label->setFont( font );

      if( isSystemBlock() ) {
        porthelper = new QNEPortHelper( this );
        label->setTextInteractionFlags( Qt::TextEditorInteraction );
      }
    } else {
      QFont font( scene()->font() );
      label->setFont( font );
    }
  }

  if( !isNoBullet() ) {
    if( isSquareBullet() ) {
      QPainterPath p;
      QPolygonF    polygon;

      polygon << QPointF( -radiusOfBullet, 0 ) << QPointF( 0, radiusOfBullet ) << QPointF( radiusOfBullet, 0 )
              << QPointF( 0, -radiusOfBullet );

      p.addPolygon( polygon );
      p.closeSubpath();
      setPath( p );

      setBrush( QColor( "yellow" ) );
      setPen( QPen( QColor( "purple" ) ) );
    } else {
      QPainterPath p;
      p.addEllipse( -radiusOfBullet, -radiusOfBullet, 2 * radiusOfBullet, 2 * radiusOfBullet );
      setPath( p );
    }
  } else {
    setPath( QPainterPath() );
  }

  recalculateLabelPosition();
}

void
QNEPort::setPortFlags( Flags flags ) {
  this->flags = flags;

  applyPortFlags();
}

qreal
QNEPort::getWidthOfLabelBoundingRect() {
  return label->boundingRect().width();
}

qreal
QNEPort::getHeightOfLabelBoundingRect() {
  return label->boundingRect().height();
}

QVariant
QNEPort::itemChange( GraphicsItemChange change, const QVariant& value ) {
  if( change == ItemScenePositionHasChanged ) {
    for( auto* conn : std::as_const( portConnections ) ) {
      conn->updatePosFromPorts();
      conn->updatePath();
    }
  }

  return value;
}

void
QNEPort::contentsChanged() {
  auto* block = qgraphicsitem_cast< QNEBlock* >( parentItem() );

  if( block != nullptr ) {
    block->block->setName( label->toPlainText() );
    block->resizeBlockWidth();
  }
}

QNEPortHelper::QNEPortHelper( QNEPort* port ) : QObject(), port( port ) {
  QObject::connect( port->label->document(), &QTextDocument::contentsChanged, this, &QNEPortHelper::contentsChanged );
}

void
QNEPortHelper::contentsChanged() {
  bool oldState = port->label->document()->blockSignals( true );
  port->contentsChanged();
  port->label->document()->blockSignals( oldState );
}
