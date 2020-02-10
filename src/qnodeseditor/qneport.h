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

#ifndef QNEPORT_H
#define QNEPORT_H

#include <QGraphicsPathItem>
#include <QTextDocument>
#include <QFontMetrics>
#include <QTextDocument>

#include <QDebug>

class QNEBlock;
class QNEConnection;

class QNEPortHelper;

class QNEPort : public QGraphicsPathItem {
  public:
    enum { Type = QGraphicsItem::UserType + 1 };
    enum { NamePort = 1, TypePort = 2, SystemBlock = 4 };

    QNEPort( QLatin1String slotSignalSignature, QGraphicsItem* parent = nullptr );
    ~QNEPort();

    void setNEBlock( QNEBlock* );
    void setName( const QString& n );
    QString getName();
    void setIsOutput( bool o );
    bool isOutput();
    std::vector<QNEConnection*>& connections();
    void setPortFlags( int );

    void contentsChanged();

    int portFlags() const {
      return m_portFlags;
    }

    int type() const override {
      return Type;
    }

    QNEBlock* block() const;

    qreal getWidthOfLabelBoundingRect() {
      return marginOfText + label->boundingRect().width();
    }

    qreal getHeightOfLabelBoundingRect() {
      QFontMetrics fm( label->font() );
      return fm.height()/* + marginOfText*/;
    }

  public:
    QLatin1String slotSignalSignature;

    static constexpr qreal radiusOfBullet = 5;
    static constexpr qreal marginOfText = 2;

    QGraphicsTextItem* label = nullptr;

  protected:
    QVariant itemChange( GraphicsItemChange change, const QVariant& value ) override;

  private:
    QNEBlock* m_block = nullptr;
    bool isOutput_ = false;
    std::vector<QNEConnection*> m_connections;
    int m_portFlags = 0;

    QNEPortHelper* porthelper = nullptr;
};

class QNEPortHelper : public QObject {
    Q_OBJECT

  public:
    QNEPortHelper( QNEPort* port )
      : QObject(), port( port ) {
      QObject::connect( port->label->document(), &QTextDocument::contentsChanged, this, &QNEPortHelper::contentsChanged );
    }

  public slots:
    void contentsChanged() {
      bool oldState = port->label->document()->blockSignals( true );
      port->contentsChanged();
      port->label->document()->blockSignals( oldState );
    }

  private:
    QNEPort* port = nullptr;
};

#endif // QNEPORT_H
