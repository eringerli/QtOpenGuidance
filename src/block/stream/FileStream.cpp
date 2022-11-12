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

#include "FileStream.h"

#include "qneblock.h"
#include "qneport.h"

#include <QBrush>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QTimerEvent>

FileStream::FileStream() { fileStream = new QTextStream(); }

FileStream::~FileStream() {
  if( file ) {
    file->deleteLater();
  }

  delete fileStream;
}

void
FileStream::setFilename( const QString& filename ) {
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
    fileStream->seek( 0 );
    timer.start( 1000 / linerate, Qt::PreciseTimer, this );
  } else {
    //    qDebug() << "fail";
  }
}

void
FileStream::setLinerate( double linerate, const CalculationOption::Options ) {
  this->linerate = linerate;

  if( qFuzzyIsNull( linerate ) ) {
    timer.stop();
  } else {
    timer.start( int( 1000 / linerate ), Qt::PreciseTimer, this );
  }
}

void
FileStream::sendData( const QByteArray& data ) {
  *fileStream << data;
}

void
FileStream::timerEvent( QTimerEvent* event ) {
  if( event->timerId() == timer.timerId() ) {
    if( file && fileStream ) {
      if( fileStream->atEnd() ) {
        timer.stop();
      } else {
        Q_EMIT dataReceived( fileStream->readLine().toLatin1() );
      }
    }
  }
}

QNEBlock*
FileStreamFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new FileStream();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "File" ), QLatin1String( SLOT( setFilename( const QString& ) ) ) );
  b->addInputPort( QStringLiteral( "Linerate" ), QLatin1String( SLOT( setLinerate( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( sendData( const QByteArray& ) ) ) );

  b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  b->setBrush( valueColor );

  return b;
}
