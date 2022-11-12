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

class CommunicationJrk : public BlockBase {
  Q_OBJECT

public:
  explicit CommunicationJrk() : BlockBase() {}

Q_SIGNALS:
  void dataReceived( const QByteArray& );

public Q_SLOTS:
  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void setSteerZero( NUMBER_SIGNATURE_SLOT );

  void setSteerCountPerDegree( NUMBER_SIGNATURE_SLOT );

private:
  float steerZero       = 2047;
  float countsPerDegree = 45;
};

class CommunicationJrkFactory : public BlockFactory {
  Q_OBJECT

public:
  CommunicationJrkFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Communication JRK" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Legacy Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
