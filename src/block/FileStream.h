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

#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <QObject>
#include <QByteArray>
#include <QTextStream>

#include "GuidanceBase.h"

class FileStream : public GuidanceBase {
    Q_OBJECT

  public:
    explicit FileStream()
      : GuidanceBase() {
      fileStream = new QTextStream();
    }

    ~FileStream() override {
      if( file ) {
        file->deleteLater();
      }

      if( fileStream ) {
        delete fileStream;
      }
    }

    void emitConfigSignals() override {
    }

  signals:
    void dataReceived( QByteArray );

  public slots:
    void setFilename( QString filename ) {
      this->filename = filename;

      if( file ) {
        fileStream->setDevice( nullptr );
        file->close();
        file->deleteLater();
        file = nullptr;
      }

      file = new QFile( filename );

      if( file->open( QFile::ReadWrite | QFile::Append | QFile::Text ) ) {
        fileStream->setDevice( file );
        qDebug() << filename;
        fileStream->seek( 0 );
        timer.start( 1000 / linerate, Qt::PreciseTimer, this );
      } else {
        qDebug() << "fail";
      }
    }

    void setLinerate( float linerate ) {
      this->linerate = linerate;

      if( qFuzzyIsNull( linerate ) ) {
        timer.stop();
      } else {
        timer.start( 1000 / linerate, Qt::PreciseTimer, this );
      }
    }

    void sendData( QByteArray data ) {
      *fileStream << data;
    }

  protected:
    void timerEvent( QTimerEvent* event ) override {
      if( event->timerId() == timer.timerId() ) {
        if( file && fileStream ) {
          if( fileStream->atEnd() ) {
            timer.stop();
          } else {
            emit dataReceived( fileStream->readLine().toLatin1() );
          }
        }
      }
    }

  public:
    QString filename;
    float linerate = 3;

  private:
    QBasicTimer timer;
    int timerId;
    QTime time;

    QTextStream* fileStream = nullptr;
    QFile* file = nullptr;
};

class FileStreamFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    FileStreamFactory()
      : GuidanceFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "File Stream" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new FileStream();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "File", SLOT( setFilename( QString ) ) );
      b->addInputPort( "Linerate", SLOT( setLinerate( float ) ) );
      b->addInputPort( "Data", SLOT( sendData( QByteArray ) ) );

      b->addOutputPort( "Data", SIGNAL( dataReceived( QByteArray ) ) );

      return b;
    }
};

#endif // FILESTREAM_H
