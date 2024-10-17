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

#include "block/BlockBase.h"
#include "qneport.h"

#include <QGraphicsPathItem>

#include <vector>

class QNEPort;
class QNEBlock
    : public QObject
    , public QGraphicsPathItem {
  Q_OBJECT

public:
  enum { Type = QGraphicsItem::UserType + 3 };

  QNEBlock( BlockBase* block, QGraphicsItem* parent = nullptr );

  void depictBlock();
  void depictConnections();

  void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

  int type() const override { return Type; }

public Q_SLOTS:
  void resizeBlockWidth();

public:
  BlockBase* block = nullptr;

  void snapToGrid();

protected:
  QVariant itemChange( GraphicsItemChange change, const QVariant& value ) override;
  void     mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) override;

private:
  QNEPort*  addPort( const QString& name, const QNEPort::Flags& flags );
  QNEPort*  addPort( const BlockPort* blockPort );
  QNEBlock* getBlockWithId( uint16_t id );
  QNEPort*  getPortWithName( const QString& name, bool output );

protected:
  static constexpr qreal horizontalMargin = 20;
  static constexpr qreal verticalMargin   = 5;
  static constexpr qreal cornerRadius     = 5;
  static constexpr qreal gradientHeight   = 10;

  static const QColor SystemBlockColor;
  static const QColor ModelColor;
  static const QColor DockColor;
  static const QColor InputDockColor;
  static const QColor ParserColor;
  static const QColor ValueColor;
  static const QColor InputOutputColor;
  static const QColor ConverterColor;
  static const QColor ArithmeticColor;
  static const QColor DefaultColor;
};
