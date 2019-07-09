// Copyright( C ) 2019 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#ifndef PRINTLATENCY_H
#define PRINTLATENCY_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include <QDateTime>

#include "GuidanceBase.h"

#include "qneblock.h"
#include "qneport.h"

#include <QElapsedTimer>

#include <QDebug>

class PrintLatency : public GuidanceBase {
    Q_OBJECT

  public:
    explicit PrintLatency()
      : GuidanceBase(),
        block( nullptr ) {}

  public slots:
    void input1() {
      timer.start();
    }

    void input2() {
      if( timer.isValid() ) {
        qDebug() << block->getName() << timer.nsecsElapsed() << "ns";
        timer.invalidate();
      }
    }

  public:
    virtual void emitConfigSignals() override {
    }

  public:
    QNEBlock* block;

  private:
    QElapsedTimer timer;
};

class PrintLatencyFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    PrintLatencyFactory()
      : GuidanceFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Print Latency" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      GuidanceBase* obj = new PrintLatency;

//      obj->moveToThread( &workerThread );

      return obj;
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      auto printLatency = qobject_cast<PrintLatency*>( obj );

      if( printLatency ) {
        printLatency->block = b;
      }

      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Input1", SLOT( input1() ) );
      b->addInputPort( "Input2", SLOT( input2() ) );

      return b;
    }
};

#endif // PRINTLATENCY_H

