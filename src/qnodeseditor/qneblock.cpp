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

#include "qneblock.h"

#include <QPen>
#include <QGraphicsScene>
#include <QFontMetrics>
#include <QPainter>

#include <QGraphicsProxyWidget>
#include <QStyleOptionGraphicsItem>

#include <QRectF>
#include <QtMath>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonValueRef>

#include <QDebug>

#include "../block/GuidanceBase.h"

#include "qneport.h"
#include "qneconnection.h"

int QNEBlock::m_nextSystemId = int( IdRange::SystemIdStart );
int QNEBlock::m_nextUserId = int( IdRange::UserIdStart );

QNEBlock::QNEBlock( QObject* object, bool systemBlock, QGraphicsItem* parent )
  : QGraphicsPathItem( parent ),
    systemBlock( systemBlock ), horzMargin( 20 ), vertMargin( 5 ), width( 20 ), height( 5 ), object( object ) {
  QPainterPath p;
  p.addRoundedRect( -60, -30, 60, 30, 5, 5 );
  setPath( p );
  setPen( QPen( Qt::darkGreen ) );
  setBrush( Qt::green );
  setFlag( QGraphicsItem::ItemIsMovable );
  setFlag( QGraphicsItem::ItemIsSelectable );

  if( systemBlock ) {
    m_id = getNextSystemId();
  } else {
    m_id = getNextUserId();
  }
}

QNEBlock::~QNEBlock() {
  object->deleteLater();
}

QNEPort* QNEBlock::addPort( const QString& name, const QString& signalSlotSignature, bool isOutput, int flags ) {
  QNEPort* port = new QNEPort( signalSlotSignature, this );
  port->setName( name );
  port->setIsOutput( isOutput );
  port->setNEBlock( this );

  if( systemBlock == true ) {
    flags |= QNEPort::SystemBlock;
  }

  if( flags & QNEPort::NamePort ) {
    this->name = name;
  }

  if( flags & QNEPort::TypePort ) {
    this->typeString = name;
  }

  port->setPortFlags( flags );

  QFontMetrics fm( scene()->font() );
  int w = fm.width( name );
  int h = fm.height();

  // port->setPos(0, height + h/2);
  if( w > ( width - horzMargin ) ) {
    width = w + horzMargin;
  }

  height += h;

  QPainterPath p;
  p.addRoundedRect( -width / 2, -height / 2, width, height, 5, 5 );
  setPath( p );

  double y = -height / 2 + vertMargin + port->radius();

  foreach( QGraphicsItem* port_, childItems() ) {
    if( port_->type() != QNEPort::Type ) {
      continue;
    }

    QNEPort* port = ( QNEPort* ) port_;

    if( port->isOutput() ) {
      port->setPos( width / 2 + port->radius(), y );

    } else {
      port->setPos( -width / 2 - port->radius(), y );
    }

    y += h;
  }

  return port;
}

void QNEBlock::addInputPort( const QString& name, const QString& signalSlotSignature ) {
  addPort( name, signalSlotSignature, false );
}

void QNEBlock::addOutputPort( const QString& name, const QString& signalSlotSignature ) {
  addPort( name, signalSlotSignature, true );
}

void QNEBlock::addWidget( QWidget* widget ) {
  QGraphicsProxyWidget* const proxy = this->scene()->addWidget( widget );
  proxy->setParentItem( this );
  double heightBuffer = height;
  height += proxy->geometry().height() + 5;
  width = qMax( proxy->geometry().width() + 10, ( double )width );
  proxy->setX( -width / 2 + ( width - proxy->geometry().width() ) / 2 );

  proxy->setY( -height / 2 + heightBuffer + 10 );
}

void QNEBlock::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) {
  Q_UNUSED( option )
  Q_UNUSED( widget )

  if( isSelected() ) {
    painter->setPen( QPen( Qt::darkYellow ) );
    painter->setBrush( Qt::yellow );
  } else {
    painter->setPen( QPen( Qt::darkGreen ) );
    painter->setBrush( Qt::green );
  }

  painter->drawPath( path() );
}

QVector<QNEPort*> QNEBlock::ports() {
  QVector<QNEPort*> res;

  foreach( QGraphicsItem* port_, childItems() ) {
    if( port_->type() == QNEPort::Type ) {
      res.append( ( QNEPort* ) port_ );
    }
  }

  return res;
}

void QNEBlock::setName( QString name ) {
  foreach( QGraphicsItem* port_, childItems() ) {
    if( port_->type() == QNEPort::Type ) {
      QNEPort* port = qgraphicsitem_cast<QNEPort*>( port_ );

      if( port && port->portFlags()&QNEPort::NamePort ) {
        port->setName( name );
      }
    }
  }

  this->name = name;
}

QVariant QNEBlock::itemChange( GraphicsItemChange change, const QVariant& value ) {

  Q_UNUSED( change )

  return value;
}

QNEPort* QNEBlock::getPortWithName( QString name, bool output ) {
  foreach( QGraphicsItem* port_, childItems() ) {
    if( port_->type() == QNEPort::Type ) {
      QNEPort* port = qgraphicsitem_cast<QNEPort*>( port_ );

      if( port &&
          !( port->portFlags() & ( QNEPort::NamePort | QNEPort::TypePort ) ) &&
          port->isOutput() == output &&
          port->getName() == name ) {
        return port;
      }
    }
  }

  return nullptr;
}

void QNEBlock::toJSON( QJsonObject& json ) {
  QJsonArray blocksArray = json["blocks"].toArray();

  QJsonObject blockObject;
  blockObject["id"] = m_id;
  blockObject["name"] = name;
  blockObject["type"] = typeString;
  blockObject["positionX"] = x();
  blockObject["positionY"] = y();

  qobject_cast<GuidanceBase*>( object )->toJSON( blockObject );

  blocksArray.append( blockObject );

  json["blocks"] = blocksArray;
}

void QNEBlock::fromJSON( QJsonObject& json ) {
  if( json["values"].isObject() ) {
    qobject_cast<GuidanceBase*>( object )->fromJSON( json );
  }
}

void QNEBlock::emitConfigSignals() {
  qobject_cast<GuidanceBase*>( object )->emitConfigSignals();
}
