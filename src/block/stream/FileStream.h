// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
  explicit FileStream( const int idHint, const bool systemBlock, const QString type );

  ~FileStream() override;

Q_SIGNALS:
  void dataReceived( const QByteArray& );

public Q_SLOTS:
  void setFilename( const QString& filename );

  void setLinerate( NUMBER_SIGNATURE_SLOT );

  void sendData( const QByteArray& data );

protected:
  void timerEvent( QTimerEvent* event ) override;

public:
  QString filename;
  float   linerate = 3;

private:
  QBasicTimer timer;

  QTextStream* fileStream = nullptr;
  QFile*       file       = nullptr;
};

class FileStreamFactory : public BlockFactory {
  Q_OBJECT

public:
  FileStreamFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::InputOutput; }

  QString getNameOfFactory() const override { return QStringLiteral( "File Stream" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Streams" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
