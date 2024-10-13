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
#include <QGraphicsPathItem>

class QNEBlock;
class QNEConnection;

class QNEPort;
class QNEPortHelper;

class QNEPort : public QGraphicsPathItem {
public:
  enum { Type = QGraphicsItem::UserType + 1 };
  enum Flag {
    None         = 0,
    NamePort     = 0x01,
    TypePort     = 0x02,
    SystemBlock  = 0x04,
    NoBullet     = 0x08,
    SquareBullet = 0x10,
    Embedded     = 0x20,
    Output       = 0x40
  };
  Q_DECLARE_FLAGS( Flags, Flag )

  QNEPort( const BlockPort* blockPort, QGraphicsItem* parent = nullptr );
  QNEPort( const QString& name, QGraphicsItem* parent = nullptr );
  virtual ~QNEPort();

  void    setName( const QString& n ) const;
  QString getName() const;

  // void       setIsOutput( bool o );
  const bool isNamePort() const { return flags.testFlag( Flag::NamePort ); };
  const bool isTypePort() const { return flags.testFlag( Flag::TypePort ); };
  const bool isSystemBlock() const { return flags.testFlag( Flag::SystemBlock ); };
  const bool isNoBullet() const { return flags.testFlag( Flag::NoBullet ); };
  const bool isSquareBullet() const { return flags.testFlag( Flag::SquareBullet ); };
  const bool isEmbedded() const { return flags.testFlag( Flag::Embedded ); };
  const bool isOutput() const { return flags.testFlag( Flag::Output ); };

  void setPortFlags( Flags flags );

  void contentsChanged();

  int type() const override { return Type; }

  qreal getWidthOfLabelBoundingRect();

  qreal getHeightOfLabelBoundingRect();

public:
  QGraphicsTextItem*            label = nullptr;
  std::vector< QNEConnection* > portConnections;

  const BlockPort* blockPort = nullptr;

  Flags flags = Flag::None;

  static constexpr qreal radiusOfBullet = 5;
  static constexpr qreal marginOfText   = 2;

  void applyPortFlags();

protected:
  QVariant itemChange( GraphicsItemChange change, const QVariant& value ) override;

private:
  QNEPortHelper* porthelper = nullptr;

  void recalculateLabelPosition();
};

class QNEPortHelper : public QObject {
  Q_OBJECT

public:
  QNEPortHelper( QNEPort* port );

public Q_SLOTS:
  void contentsChanged();

private:
  QNEPort* port = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QNEPort::Flags )
