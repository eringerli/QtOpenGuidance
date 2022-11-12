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

class CommunicationPgn7ffe : public BlockBase {
  Q_OBJECT

public:
  explicit CommunicationPgn7ffe() : BlockBase() {}

Q_SIGNALS:
  void dataReceived( const QByteArray& );

public Q_SLOTS:
  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void setXte( NUMBER_SIGNATURE_SLOT );

  void setVelocity( NUMBER_SIGNATURE_SLOT );

private:
  float distance = 0;
  float velocity = 0;
};

class CommunicationPgn7ffeFactory : public BlockFactory {
  Q_OBJECT

public:
  CommunicationPgn7ffeFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Communication PGN 7FFE" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Legacy Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
