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

#ifndef QNEBLOCK_H
#define QNEBLOCK_H

#include <QGraphicsPathItem>

class QNEPort;

class QNEBlock : public QGraphicsPathItem {
    Q_GADGET

  public:
    enum { Type = QGraphicsItem::UserType + 3 };

    enum class IdRange {
      SystemIdStart = 1,
      UserIdStart = 1000
    };

    // QNEBlock takes ownership of the given QObject -> it deletes it in its destructor
    QNEBlock( QObject* object, bool systemBlock = false, QGraphicsItem* parent = nullptr );

    ~QNEBlock();

    QNEPort* addPort( const QString& name, const QString& signalSlotSignature, bool isOutput, int flags = 0 );
    void addInputPort( const QString& name, const QString& signalSlotSignature );
    void addOutputPort( const QString& name, const QString& signalSlotSignature );

    void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget ) override;

    void toJSON( QJsonObject& json );
    void fromJSON( QJsonObject& json );
    void emitConfigSignals();

    QVector<QNEPort*> ports();

    int type() const override {
      return Type;
    }

    void setName( const QString& str, bool setFromLabel = false );
    void setType( QString str );

    QNEPort* getPortWithName( const QString& name, bool output );

    bool systemBlock = false;

  public:
    int getNextSystemId() {
      return m_nextSystemId++;
    }
    int getNextUserId() {
      return m_nextUserId++;
    }

  public slots:
    void resizeBlockWidth();

  public:
    int id = 0;

    static constexpr qreal horizontalMargin = 20;
    static constexpr qreal verticalMargin = 5;
    static constexpr qreal cornerRadius = 5;

  private:
    static int m_nextSystemId;
    static int m_nextUserId;

  protected:
    QVariant itemChange( GraphicsItemChange change, const QVariant& value ) override;
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) override;

  private:
    qreal width;
    qreal height;
    QString name;

  public:
    QString getName() {
      return name;
    }

  public:
    QObject* object;
    QString typeString;
};

#endif // QNEBLOCK_H
