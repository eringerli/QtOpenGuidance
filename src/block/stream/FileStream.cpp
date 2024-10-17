// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FileStream.h"

#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QTimerEvent>

FileStream::FileStream( const BlockBaseId idHint, const bool systemBlock, const QString type, const BlockBase::TypeColor typeColor )
    : BlockBase( idHint, systemBlock, type, typeColor ) {
  fileStream = new QTextStream();
}

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

std::unique_ptr< BlockBase >
FileStreamFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< FileStream >( idHint );

  obj->addInputPort( QStringLiteral( "File" ), obj.get(), QLatin1StringView( SLOT( setFilename( const QString& ) ) ) );
  obj->addInputPort( QStringLiteral( "Linerate" ), obj.get(), QLatin1StringView( SLOT( setLinerate( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Data" ), obj.get(), QLatin1StringView( SLOT( sendData( const QByteArray& ) ) ) );

  obj->addOutputPort( QStringLiteral( "Data" ), obj.get(), QLatin1StringView( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  return obj;
}
