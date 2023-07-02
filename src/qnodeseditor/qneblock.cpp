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
#include <QGraphicsItem>
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

#include "qneconnection.h"
#include "qneport.h"

#include "block/BlockBase.h"

int QNEBlock::m_nextSystemId = int( IdRange::SystemIdStart );
int QNEBlock::m_nextUserId   = int( IdRange::UserIdStart );

QNEBlock::QNEBlock( QObject* object, int id, bool systemBlock, QGraphicsItem* parent )
    : QGraphicsPathItem( parent ), systemBlock( systemBlock ) {
  addObject( object );

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
  for( auto& object : objects ) {
    object->deleteLater();
  }
}

QNEPort*
QNEBlock::addPort( const QString& name, QLatin1String signalSlotSignature, bool isOutput, int flags, bool embedded ) {
  auto* port = new QNEPort( signalSlotSignature, this, embedded );
  port->setName( name );
  port->setIsOutput( isOutput );

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

  resizeBlockWidth();

  return port;
}

void
QNEBlock::addInputPort( const QString& name, QLatin1String signalSlotSignature, bool embedded ) {
  addPort( name, signalSlotSignature, false, 0, embedded );
}

void
QNEBlock::addOutputPort( const QString& name, QLatin1String signalSlotSignature, bool embedded ) {
  addPort( name, signalSlotSignature, true, 0, embedded );
}

QVariant
QNEBlock::itemChange( QGraphicsItem::GraphicsItemChange change, const QVariant& value ) {
  if( change == GraphicsItemChange::ItemSelectedHasChanged && scene() ) {
    for( const auto child : childItems() ) {
      if( const auto* port = qgraphicsitem_cast< QNEPort* >( child ) ) {
        for( auto* connection : port->portConnections ) {
          connection->highlight( value.toBool() );
        }
      }
    }
  }
  return QGraphicsItem::itemChange( change, value );
}

void
QNEBlock::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) {
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

void
QNEBlock::setName( const QString& name, bool setFromLabel ) {
  if( !setFromLabel ) {
    const auto& constRefOfList = childItems();

    for( const auto& item : constRefOfList ) {
      auto* port = qgraphicsitem_cast< QNEPort* >( item );

      if( ( port != nullptr ) && ( ( port->portFlags() & QNEPort::NamePort ) != 0 ) ) {
        port->setName( name );
      }
    }
  }

  this->name = name;

  for( auto& object : objects ) {
    auto* obj = qobject_cast< BlockBase* >( object );

    if( obj != nullptr ) {
      obj->setName( name );
    }
  }

  resizeBlockWidth();
}

QNEPort*
QNEBlock::getPortWithName( const QString& name, bool output ) {
  const auto& constRefOfList = childItems();

  for( const auto& item : constRefOfList ) {
    auto* port = qgraphicsitem_cast< QNEPort* >( item );

    if( ( port != nullptr ) && ( ( port->portFlags() & ( QNEPort::NamePort | QNEPort::TypePort ) ) == 0 ) && port->isOutput() == output &&
        port->getName() == name ) {
      return port;
    }
  }

  return nullptr;
}

void
QNEBlock::addObject( QObject* object ) {
  objects.push_back( object );
}

QJsonObject
QNEBlock::toJSON() const {
  QJsonObject blockObject;
  blockObject[QStringLiteral( "id" )]        = id;
  blockObject[QStringLiteral( "name" )]      = name;
  blockObject[QStringLiteral( "type" )]      = typeString;
  blockObject[QStringLiteral( "positionX" )] = x();
  blockObject[QStringLiteral( "positionY" )] = y();

  {
    int i = 0;
    for( auto& object : objects ) {
      auto obj = qobject_cast< BlockBase* >( object );
      if( obj ) {
        auto json = obj->toJSON();
        if( !json.empty() ) {
          blockObject[QStringLiteral( "value" ) + QString::number( i )] = json;
        }
      }
      ++i;
    }
  }

  return blockObject;
}

void
QNEBlock::fromJSON( QJsonObject& json ) {
  setX( json[QStringLiteral( "positionX" )].toDouble( 0 ) );
  setY( json[QStringLiteral( "positionY" )].toDouble( 0 ) );

  if( json[QStringLiteral( "values" )].isObject() ) {
    auto jsonValue = json[QStringLiteral( "values" )].toObject();
    qobject_cast< BlockBase* >( objects.front() )->fromJSON( jsonValue );
  }

  int i = 0;
  for( auto& object : objects ) {
    if( json[QStringLiteral( "value" ) + QString::number( i )].isObject() ) {
      auto jsonValue = json[QStringLiteral( "value" ) + QString::number( i )].toObject();
      qobject_cast< BlockBase* >( object )->fromJSON( jsonValue );
    }
    ++i;
  }
}

void
QNEBlock::resizeBlockWidth() {
  QFontMetrics fm( scene()->font() );

  double gridSpacing = fm.height();

  double width  = 0;
  double height = gridSpacing * childItems().size();

  {
    const auto& constRefOfList = childItems();

    for( const auto& item : constRefOfList ) {
      auto* port = qgraphicsitem_cast< QNEPort* >( item );

      if( port != nullptr ) {
        width = std::max( width, port->getWidthOfLabelBoundingRect() );
      }
    }
  }

  width = std::ceil( width / ( gridSpacing ) ) * ( gridSpacing ) + cornerRadius;

  double y = gridSpacing / 2;

  {
    const auto& constRefOfList = childItems();

    for( const auto& item : constRefOfList ) {
      auto* port = qgraphicsitem_cast< QNEPort* >( item );

      if( port != nullptr ) {
        if( port->isOutput() ) {
          port->setPos( width + cornerRadius + 1, y );
        } else {
          port->setPos( -cornerRadius - 1, y );
        }

        y += gridSpacing;
      }
    }
  }

  QPainterPath p;

  p.addRoundedRect( 0, 0, width, height, cornerRadius, cornerRadius );
  setPath( p );
}

void
QNEBlock::mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
  QFontMetrics fm( scene()->font() );
  double       gridSpacing = double( fm.height() );

  double xx = x();
  double yy = y();

  xx = gridSpacing * std::round( xx / gridSpacing );
  yy = gridSpacing * std::round( yy / gridSpacing );

  setPos( xx, yy );

  QGraphicsItem::mouseReleaseEvent( event );
}
