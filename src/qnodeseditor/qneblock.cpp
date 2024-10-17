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

#include <QMetaMethod>

#include "qneconnection.h"
#include "qneport.h"

#include "block/BlockBase.h"
#include "helpers/BlocksManager.h"

QNEBlock::QNEBlock( BlockBase* block, QGraphicsItem* parent ) : QGraphicsPathItem( parent ), block( block ) {
  QPainterPath p;
  p.addRoundedRect( -60, -30, 60, 30, cornerRadius, cornerRadius );
  setPath( p );
  setPen( QPen( Qt::darkGreen ) );

  if( block->systemBlock ) {
    setBrush( SystemBlockColor );
  } else {
    switch( block->typeColor ) {
      case BlockBase::TypeColor::Model:
        setBrush( ModelColor );
        break;
      case BlockBase::TypeColor::Dock:
        setBrush( DockColor );
        break;
      case BlockBase::TypeColor::InputDock:
        setBrush( InputDockColor );
        break;
      case BlockBase::TypeColor::Parser:
        setBrush( ParserColor );
        break;
      case BlockBase::TypeColor::Value:
        setBrush( ValueColor );
        break;
      case BlockBase::TypeColor::InputOutput:
        setBrush( InputOutputColor );
        break;
      case BlockBase::TypeColor::Converter:
        setBrush( ConverterColor );
        break;
      case BlockBase::TypeColor::Arithmetic:
        setBrush( ArithmeticColor );
        break;
      case BlockBase::TypeColor::System:
        setBrush( SystemBlockColor );
        break;

      default:
        setBrush( DefaultColor );
    }
  }

  setX( block->positionX );
  setY( block->positionY );

  setFlag( QGraphicsItem::ItemIsMovable );
  setFlag( QGraphicsItem::ItemIsSelectable );
  setFlag( QGraphicsItem::ItemSendsGeometryChanges );
}

void
QNEBlock::depictBlock() {
  QNEPort::Flags flags;

  flags.setFlag( QNEPort::Flag::SystemBlock, block->systemBlock );

  auto sortedPorts = block->ports();

  addPort( block->name(), flags | QNEPort::NamePort | QNEPort::NoBullet );
  addPort( block->type, flags | QNEPort::TypePort | QNEPort::NoBullet );

  for( const auto& port : block->ports() ) {
    if( port.isVisible() ) {
      addPort( &port );
    }
  }

  resizeBlockWidth();
}

void
QNEBlock::depictConnections() {
  for( const auto& connection : block->connections() ) {
    if( connection.portFrom->isVisible() && connection.portTo->isVisible() ) {
      auto* portFrom = getPortWithName( connection.portFrom->name, true );

      if( portFrom != nullptr ) {
        auto* blockTo = getBlockWithId( connection.portTo->idOfBlock );

        if( blockTo != nullptr ) {
          auto* portTo = blockTo->getPortWithName( connection.portTo->name, false );

          if( portTo != nullptr ) {
            auto* qneConnection = new QNEConnection( &connection, portFrom, portTo );
            portFrom->portConnections.push_back( qneConnection );
            portTo->portConnections.push_back( qneConnection );

            scene()->addItem( qneConnection );
          } else {
            qDebug() << "portTo != nullptr";
          }
        } else {
          qDebug() << "blockTo != nullptr";
        }
      } else {
        qDebug() << "portFrom != nullptr";
      }
    } else {
      qDebug() << "connection.from->visible && connection.to->visible";
    }
  }
}

QNEPort*
QNEBlock::addPort( const QString& name, const QNEPort::Flags& flags ) {
  auto* port = new QNEPort( name, this );

  port->setPortFlags( flags );

  return port;
}

QNEPort*
QNEBlock::addPort( const BlockPort* blockPort ) {
  auto* port = new QNEPort( blockPort, this );

  return port;
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

  if( change == ItemPositionChange && scene() ) {
    QFontMetrics fm( scene()->font() );
    double       gridSpacing = double( fm.height() );

    QPointF newPos = value.toPointF();
    double  xx     = newPos.x();
    double  yy     = newPos.y();

    xx = gridSpacing * std::round( xx / gridSpacing );
    yy = gridSpacing * std::round( yy / gridSpacing );

    return QPointF( xx, yy );
  }

  return QGraphicsItem::itemChange( change, value );
}

void
QNEBlock::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) {
  Q_UNUSED( option )
  Q_UNUSED( widget )

  if( isSelected() ) {
    painter->setPen( QPen( Qt::darkYellow ) );

    if( block->systemBlock ) {
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

QNEPort*
QNEBlock::getPortWithName( const QString& name, bool output ) {
  const auto& constRefOfList = childItems();

  for( const auto& item : constRefOfList ) {
    auto* port = qgraphicsitem_cast< QNEPort* >( item );

    if( ( port != nullptr ) &&
        ( !port->isNamePort() && !port->isTypePort() && ( port->isOutput() == output ) && ( port->getName() == name ) ) ) {
      return port;
    }
  }

  return nullptr;
}

QNEBlock*
QNEBlock::getBlockWithId( uint16_t id ) {
  const auto& constRefOfList = scene()->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( block->block->id() == id ) {
        return block;
      }
    }
  }

  return nullptr;
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
QNEBlock::snapToGrid() {
  QFontMetrics fm( scene()->font() );
  double       gridSpacing = double( fm.height() );

  double xx = x();
  double yy = y();

  xx = gridSpacing * std::round( xx / gridSpacing );
  yy = gridSpacing * std::round( yy / gridSpacing );

  setPos( xx, yy );

  block->positionX = xx;
  block->positionY = yy;
}

void
QNEBlock::mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
  for( auto& item : scene()->items() ) {
    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

    if( block != nullptr ) {
      if( block->isSelected() ) {
        block->snapToGrid();
      }
    }
  }

  QGraphicsItem::mouseReleaseEvent( event );
}

const QColor QNEBlock::SystemBlockColor = QColor( Qt::lightGray );
const QColor QNEBlock::ModelColor       = QColor( QStringLiteral( "moccasin" ) );
const QColor QNEBlock::DockColor        = QColor( QStringLiteral( "DarkSalmon" ) );
const QColor QNEBlock::InputDockColor   = QColor( QStringLiteral( "lightsalmon" ) );
const QColor QNEBlock::ParserColor      = QColor( QStringLiteral( "mediumaquamarine" ) );
const QColor QNEBlock::ValueColor       = QColor( QStringLiteral( "gold" ) );
const QColor QNEBlock::InputOutputColor = QColor( QStringLiteral( "cornflowerblue" ) );
const QColor QNEBlock::ConverterColor   = QColor( QStringLiteral( "lightblue" ) );
const QColor QNEBlock::ArithmeticColor  = QColor( QStringLiteral( "DarkKhaki" ) );
const QColor QNEBlock::DefaultColor     = QColor( QStringLiteral( "DarkKhaki" ) );
