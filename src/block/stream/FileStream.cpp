// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
