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

#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <QObject>
#include <QByteArray>
#include <QTextStream>

#include "BlockBase.h"

class FileStream : public BlockBase {
    Q_OBJECT

  public:
    explicit FileStream()
      : BlockBase() {
      fileStream = new QTextStream();
    }

    ~FileStream() override {
      if( file ) {
        file->deleteLater();
      }

      delete fileStream;
    }

    void emitConfigSignals() override {
    }

  signals:
    void dataReceived( const QByteArray& );

  public slots:
    void setFilename( QString& filename ) {
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
        timer.start( int( 1000 / linerate ), Qt::PreciseTimer, this );
      }
    }

    void sendData( const QByteArray& data ) {
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

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene ) override {
      auto* obj = new FileStream();
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( QStringLiteral( "File" ), QLatin1String( SLOT( setFilename( const QString& ) ) ) );
      b->addInputPort( QStringLiteral( "Linerate" ), QLatin1String( SLOT( setLinerate( float ) ) ) );
      b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( sendData( const QByteArray& ) ) ) );

      b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

      return b;
    }
};

#endif // FILESTREAM_H
