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
 * DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qneport.h"
#include "qneblock.h"

#include <QGraphicsScene>
#include <QFontMetrics>

#include <QPen>

#include <QEvent>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>
#include <utility>
#include <utility>

#include "qneconnection.h"

QNEPort::QNEPort( QLatin1String slotSignalSignature, QGraphicsItem* parent ):
  QGraphicsPathItem( parent ) {

  this->slotSignalSignature = slotSignalSignature;
  label = new QGraphicsTextItem( this );

  QPainterPath p;
  p.addEllipse( -radiusOfBullet, -radiusOfBullet, 2 * radiusOfBullet, 2 * radiusOfBullet );
  setPath( p );

  setPen( QPen( Qt::darkRed ) );
  setBrush( Qt::red );

  setFlag( QGraphicsItem::ItemSendsScenePositionChanges );
}

QNEPort::~QNEPort() {
  for( auto conn : qAsConst( m_connections ) ) {
    delete conn;
  }

  label->deleteLater();

  if( porthelper != nullptr ) {
    porthelper->deleteLater();
  }
}

void QNEPort::setNEBlock( QNEBlock* b ) {
  m_block = b;
}

void QNEPort::setName( const QString& n ) {
  label->setPlainText( n );
}

QString QNEPort::getName() {
  return label->toPlainText();
}

void QNEPort::setIsOutput( bool output ) {
  isOutput_ = output;

  if( isOutput_ ) {
    label->setPos( -radiusOfBullet - marginOfText - label->boundingRect().width(), -label->boundingRect().height() / 2 );

  } else {
    label->setPos( radiusOfBullet + marginOfText, -label->boundingRect().height() / 2 );
  }
}

bool QNEPort::isOutput() {
  return isOutput_;
}

std::vector<QNEConnection*>& QNEPort::connections() {
  return m_connections;
}

void QNEPort::setPortFlags( int f ) {
  m_portFlags = f;

  if( ( m_portFlags & TypePort ) != 0 ) {
    QFont font( scene()->font() );
    font.setItalic( true );
    label->setFont( font );
    setPath( QPainterPath() );
  } else if( ( m_portFlags & NamePort ) != 0 ) {
    QFont font( scene()->font() );
    font.setBold( true );
    label->setFont( font );
    setPath( QPainterPath() );

    if( ( m_portFlags & QNEPort::SystemBlock ) == 0 ) {
      porthelper = new QNEPortHelper( this );
      label->setTextInteractionFlags( Qt::TextEditorInteraction );
    }
  }
}

QNEBlock* QNEPort::block() const {
  return m_block;
}

QVariant QNEPort::itemChange( GraphicsItemChange change, const QVariant& value ) {
  if( change == ItemScenePositionHasChanged ) {
    for( auto conn : qAsConst( m_connections ) ) {
      conn->updatePosFromPorts();
      conn->updatePath();
    }
  }

  return value;
}

void QNEPort::contentsChanged() {
  auto* block = qgraphicsitem_cast<QNEBlock*>( parentItem() );

  if( block != nullptr ) {
    block->setName( label->toPlainText(), true );
    block->resizeBlockWidth();
  }
}
