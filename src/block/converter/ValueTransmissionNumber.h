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

#include "ValueTransmissionBase.h"

#include <memory>

class QCborStreamReader;

class ValueTransmissionNumber : public ValueTransmissionBase {
  Q_OBJECT
public:
  explicit ValueTransmissionNumber( int );

public Q_SLOTS:
  void setNumber( NUMBER_SIGNATURE_SLOT );
  void dataReceive( const QByteArray& );

Q_SIGNALS:
  void dataToSend( const QByteArray& );
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  std::unique_ptr< QCborStreamReader > reader;
};

class ValueTransmissionNumberFactory : public BlockFactory {
  Q_OBJECT

public:
  ValueTransmissionNumberFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Value Transmit Number" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Value Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
