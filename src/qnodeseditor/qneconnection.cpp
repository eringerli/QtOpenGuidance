/* Copyright (c) 2012, STANISLAW ADASZEWSKI
 * Copyright (c) 2019, Christian Riggenbach
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

#include "qneconnection.h"

#include "qneport.h"
#include "qneblock.h"

#include <QObject>
#include <QBrush>
#include <QPen>
#include <QGraphicsScene>
#include <QPainter>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonValueRef>

QNEConnection::QNEConnection( QGraphicsItem* parent ) : QGraphicsPathItem( parent ) {
  setPen( QPen( Qt::black, 2 ) );
  setBrush( Qt::NoBrush );
  setZValue( -1 );
  m_port1 = nullptr;
  m_port2 = nullptr;
  setFlag( QGraphicsItem::ItemIsSelectable );
}

QNEConnection::~QNEConnection() {
  if( m_port1 ) {
    m_port1->connections().remove( m_port1->connections().indexOf( this ) );
  }

  if( m_port2 ) {
    m_port2->connections().remove( m_port2->connections().indexOf( this ) );
  }

  QObject::disconnect( connection );
}


void QNEConnection::paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* ) {
  if( isSelected() ) {
    painter->setPen( QPen( Qt::red, 3 ) );
    painter->setBrush( Qt::NoBrush );
    setZValue( 1.5 );
  } else {
    painter->setPen( QPen( Qt::darkGreen, 3 ) );
    painter->setBrush( Qt::NoBrush );
    setZValue( 0 );
  }

  painter->drawPath( path() );
}

void QNEConnection::setPos1( const QPointF& p ) {
  pos1 = p;
}

void QNEConnection::setPos2( const QPointF& p ) {
  pos2 = p;
}


void QNEConnection::setPort1( QNEPort* p ) {
  m_port1 = p;

  m_port1->connections().append( this );
}

bool QNEConnection::setPort2( QNEPort* p ) {

  connection = QObject::connect( m_port1->block()->object, ( const char* )( m_port1->slotSignalSignature.toUtf8().constData() ),
                                 p->block()->object, ( const char* )( p->slotSignalSignature.toUtf8().constData() ),
                                 Qt::ConnectionType( Qt::AutoConnection | Qt::UniqueConnection ) );

  if( ( bool )connection ) {
    m_port2 = p;
    m_port2->connections().append( this );

    return true;
  } else {
    return false;
  }
}

void QNEConnection::updatePosFromPorts() {
  pos1 = m_port1->scenePos();
  pos2 = m_port2->scenePos();
}

void QNEConnection::updatePath() {
  QPainterPath p;

  p.moveTo( pos1 );

  qreal dx = pos2.x() - pos1.x();
  qreal dy = pos2.y() - pos1.y();

  QPointF ctr1( pos1.x() + dx * 0.25, pos1.y() + dy * 0.1 );
  QPointF ctr2( pos1.x() + dx * 0.75, pos1.y() + dy * 0.9 );

  p.cubicTo( ctr1, ctr2, pos2 );

  setPath( p );
}

QNEPort* QNEConnection::port1() const {
  return m_port1;
}

QNEPort* QNEConnection::port2() const {
  return m_port2;
}

void QNEConnection::toJSON( QJsonObject& json ) {
  QJsonArray connectionsArray = json["connections"].toArray();

  QJsonObject connectionObject;
  connectionObject["idFrom"] = port1()->block()->id;
  connectionObject["portFrom"] = port1()->getName();
  connectionObject["idTo"] =  port2()->block()->id;
  connectionObject["portTo"] = port2()->getName();
  connectionsArray.append( connectionObject );

  json["connections"] = connectionsArray;
}
