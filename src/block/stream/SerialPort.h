// Copyright( C ) 2020 Christian Riggenbach
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

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class QSerialPort;

class SerialPort : public BlockBase {
    Q_OBJECT

  public:
    explicit SerialPort();
    ~SerialPort();

  signals:
    void  dataReceived( const QByteArray& );

  public slots:
    void setPort( const QString& port );

    void setBaudrate( double baudrate );

    void sendData( const QByteArray& data );

  protected slots:
    void processPendingData();

  public:
    QString port;
    double baudrate = 0;

  private:
    QSerialPort* serialPort = nullptr;
};

class SerialPortFactory : public BlockFactory {
    Q_OBJECT

  public:
    SerialPortFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Serial Port" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Streams" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
