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

#include "QBasicTimer"

class QFile;
class QByteArray;
class QTextStream;

class FileStream : public BlockBase {
    Q_OBJECT

  public:
    explicit FileStream();

    ~FileStream() override;

  Q_SIGNALS:
    void dataReceived( const QByteArray& );

  public Q_SLOTS:
    void setFilename( const QString& filename );

    void setLinerate( double linerate );

    void sendData( const QByteArray& data );

  protected:
    void timerEvent( QTimerEvent* event ) override;

  public:
    QString filename;
    float linerate = 3;

  private:
    QBasicTimer timer;

    QTextStream* fileStream = nullptr;
    QFile* file = nullptr;
};

class FileStreamFactory : public BlockFactory {
    Q_OBJECT

  public:
    FileStreamFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "File Stream" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Streams" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
