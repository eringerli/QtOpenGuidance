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
  explicit FileStream();

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
  FileStreamFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "File Stream" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Streams" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
