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

#include "helpers/eigenHelper.h"

class NmeaParserRMC : public BlockBase {
    Q_OBJECT

  public:
    explicit NmeaParserRMC()
      : BlockBase() {
    }

  Q_SIGNALS:
    void globalPositionChanged( const Eigen::Vector3d& );
    void velocityChanged( const double );

  public Q_SLOTS:
    void setData( const QByteArray& data );

  public:
    void parseData();

  private:
    QByteArray dataToParse;
};

class NmeaParserRMCFactory : public BlockFactory {
    Q_OBJECT

  public:
    NmeaParserRMCFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "NMEA Parser RMC" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Parsers" );
    }

    QString getPrettyNameOfFactory() override {
      return QStringLiteral( "NMEA Parser for RMC sentences" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
