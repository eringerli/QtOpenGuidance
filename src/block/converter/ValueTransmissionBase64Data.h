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

class ValueTransmissionBase64Data : public ValueTransmissionBase {
  Q_OBJECT
public:
  explicit ValueTransmissionBase64Data( int id );

public Q_SLOTS:
  void setData( const QByteArray& data );

  void dataReceive( const QByteArray& data );

Q_SIGNALS:
  void dataToSend( const QByteArray& );
  void dataChanged( const QByteArray& );

private:
  std::unique_ptr< QCborStreamReader > reader;
};

class ValueTransmissionBase64DataFactory : public BlockFactory {
  Q_OBJECT

public:
  ValueTransmissionBase64DataFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Value Transmit Base64 Data" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Value Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
