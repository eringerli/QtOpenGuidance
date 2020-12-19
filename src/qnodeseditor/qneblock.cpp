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

#include "qneblock.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsScene>
#include <QPainter>
#include <QPen>

#include <QGraphicsProxyWidget>
#include <QStyleOptionGraphicsItem>

#include <QRectF>
#include <QtMath>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>

#include <QDebug>

#include "block/BlockBase.h"

#include "qneconnection.h"
#include "qneport.h"

int QNEBlock::m_nextSystemId = int( IdRange::SystemIdStart );
int QNEBlock::m_nextUserId = int( IdRange::UserIdStart );

QNEBlock::QNEBlock( QObject* object, int id, bool systemBlock, QGraphicsItem* parent )
  : QGraphicsPathItem( parent ),
    systemBlock( systemBlock ), width( 20 ), height( cornerRadius * 2 ), object( object ) {
  QPainterPath p;
  p.addRoundedRect( -60, -30, 60, 30, cornerRadius, cornerRadius );
  setPath( p );
  setPen( QPen( Qt::darkGreen ) );

  if( systemBlock ) {
    setBrush( Qt::lightGray );
  } else {
    setBrush( QColor( "palegreen" ) );
  }

  setFlag( QGraphicsItem::ItemIsMovable );
  setFlag( QGraphicsItem::ItemIsSelectable );

  if( id == 0 ) {
    if( systemBlock ) {
      this->id = getNextSystemId();
    } else {
      this->id = getNextUserId();
    }
  } else {
    this->id = id;

    if( systemBlock ) {
      if( m_nextSystemId < id ) {
        m_nextSystemId = id + 1;
      }
    } else {
      if( m_nextUserId < id ) {
        m_nextUserId = id + 1;
      }
    }
  }
}

QNEBlock::~QNEBlock() {
  object->deleteLater();
}

QNEPort* QNEBlock::addPort( const QString& name, QLatin1String signalSlotSignature, bool isOutput, int flags, bool embedded ) {
  auto* port = new QNEPort( signalSlotSignature, this, embedded );
  port->setName( name );
  port->setIsOutput( isOutput );
  port->setNEBlock( this );

  if( systemBlock ) {
    flags |= QNEPort::SystemBlock;
  }

  if( ( flags & QNEPort::NamePort ) != 0 ) {
    this->name = name;
  }

  if( ( flags & QNEPort::TypePort ) != 0 ) {
    this->typeString = name;
  }

  port->setPortFlags( flags );

  height += port->getHeightOfLabelBoundingRect();

  resizeBlockWidth();

  return port;
}

void QNEBlock::addInputPort( const QString& name, QLatin1String signalSlotSignature, bool embedded ) {
  addPort( name, signalSlotSignature, false, 0, embedded );
}

void QNEBlock::addOutputPort( const QString& name, QLatin1String signalSlotSignature, bool embedded ) {
  addPort( name, signalSlotSignature, true, 0, embedded );
}

void QNEBlock::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) {
  Q_UNUSED( option )
  Q_UNUSED( widget )

  if( isSelected() ) {
    painter->setPen( QPen( Qt::darkYellow ) );

    if( systemBlock ) {
      painter->setBrush( Qt::darkGray );
    } else {
      painter->setBrush( Qt::yellow );
    }

    setZValue( 1 );
  } else {
    painter->setPen( pen() );
    painter->setBrush( brush() );

    setZValue( 0.5 );
  }

  painter->drawPath( path() );
}

void QNEBlock::setName( const QString& name, bool setFromLabel ) {
  if( !setFromLabel ) {
    const auto& constRefOfList = childItems();

    for( const auto& item : constRefOfList ) {
      auto* port = qgraphicsitem_cast<QNEPort*>( item );

      if( ( port != nullptr ) && ( ( port->portFlags()&QNEPort::NamePort ) != 0 ) ) {
        port->setName( name );
      }
    }
  }

  this->name = name;

  auto* obj = qobject_cast<BlockBase*>( object );

  if( obj != nullptr ) {
    obj->setName( name );
  }

  resizeBlockWidth();
}

QVariant QNEBlock::itemChange( GraphicsItemChange change, const QVariant& value ) {

  Q_UNUSED( change )

  return value;
}

QNEPort* QNEBlock::getPortWithName( const QString& name, bool output ) {
  const auto& constRefOfList = childItems();

  for( const auto& item : constRefOfList ) {
    auto* port = qgraphicsitem_cast<QNEPort*>( item );

    if( ( port != nullptr ) &&
        ( ( port->portFlags() & ( QNEPort::NamePort | QNEPort::TypePort ) ) == 0 ) &&
        port->isOutput() == output &&
        port->getName() == name ) {
      return port;
    }
  }

  return nullptr;
}

void QNEBlock::toJSON( QJsonObject& json ) {
  QJsonArray blocksArray = json[QStringLiteral( "blocks" )].toArray();

  QJsonObject blockObject;
  blockObject[QStringLiteral( "id" )] = id;
  blockObject[QStringLiteral( "name" )] = name;
  blockObject[QStringLiteral( "type" )] = typeString;
  blockObject[QStringLiteral( "positionX" )] = x();
  blockObject[QStringLiteral( "positionY" )] = y();

  qobject_cast<BlockBase*>( object )->toJSON( blockObject );

  blocksArray.append( blockObject );

  json[QStringLiteral( "blocks" )] = blocksArray;
}

void QNEBlock::fromJSON( QJsonObject& json ) {
  if( json[QStringLiteral( "values" )].isObject() ) {
    qobject_cast<BlockBase*>( object )->fromJSON( json );
  }
}

void QNEBlock::resizeBlockWidth() {
  QFontMetrics fm( scene()->font() );
  qreal gridSpacing = fm.height() * 2;

  qreal heightSnappedToGridSpacing = round( ( height - 2 * cornerRadius ) / ( gridSpacing ) ) * ( gridSpacing ) + 2 * cornerRadius;

  double y = -heightSnappedToGridSpacing / 2 + verticalMargin + cornerRadius;
  width = 0;

  {
    const auto& constRefOfList = childItems();

    for( const auto& item : constRefOfList ) {
      auto* port = qgraphicsitem_cast<QNEPort*>( item );

      if( port != nullptr ) {
        if( width < port->getWidthOfLabelBoundingRect() ) {
          width = port->getWidthOfLabelBoundingRect();
        }
      }
    }
  }

  qreal widthSnappedToGridSpacing = ceil( width / ( gridSpacing ) ) * ( gridSpacing );

  {
    const auto& constRefOfList = childItems();

    for( const auto& item : constRefOfList ) {
      auto* port = qgraphicsitem_cast<QNEPort*>( item );

      if( port != nullptr ) {
        if( port->isOutput() ) {
          port->setPos( widthSnappedToGridSpacing / 2 + cornerRadius, y );
        } else {
          port->setPos( -widthSnappedToGridSpacing / 2 - cornerRadius, y );
        }

        y += port->getHeightOfLabelBoundingRect();
      }
    }
  }

  QPainterPath p;
  double offset = 0;

  if( ( childItems().size() % 2 ) != 0 ) {
    offset = gridSpacing / 4;
  }

  p.addRoundedRect( -widthSnappedToGridSpacing / 2, ( -height / 2 ) - offset, widthSnappedToGridSpacing, height, cornerRadius, cornerRadius );
  setPath( p );
}

void QNEBlock::mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
  QFontMetrics fm( scene()->font() );
  qreal gridSpacing = fm.height();

  qreal xx = x();
  qreal yy = y();

  if( ( int( xx ) % int( gridSpacing ) ) != 0 ) {
    xx = gridSpacing * round( xx / gridSpacing );
  }

  if( ( int( yy ) % int( gridSpacing ) ) != 0 ) {
    yy = gridSpacing * round( yy / gridSpacing );
  }

  setPos( xx, yy );

  QGraphicsItem::mouseReleaseEvent( event );
}
