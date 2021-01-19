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

#include "helpers/eigenHelper.h"

class QCborStreamReader;

class ValueTransmissionQuaternion : public ValueTransmissionBase {
    Q_OBJECT
  public:
    explicit ValueTransmissionQuaternion( int id );

  public Q_SLOTS:
    void setQuaternion( const Eigen::Quaterniond& quaternion );

    void dataReceive( const QByteArray& data );

  Q_SIGNALS:
    void dataToSend( const QByteArray& );
    void quaternionChanged( const Eigen::Quaterniond );

  private:
    std::unique_ptr<QCborStreamReader> reader;
};

class ValueTransmissionQuaternionFactory : public BlockFactory {
    Q_OBJECT

  public:
    ValueTransmissionQuaternionFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Value Transmit Quaternion" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Value Converters" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
