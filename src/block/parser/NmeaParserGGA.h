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

class NmeaParserGGA : public BlockBase {
  Q_OBJECT

public:
  explicit NmeaParserGGA() : BlockBase() {}

Q_SIGNALS:
  void globalPositionChanged( const Eigen::Vector3d& );
  void fixQualityChanged( NUMBER_SIGNATURE_SIGNAL );
  void hdopChanged( NUMBER_SIGNATURE_SIGNAL );
  void numSatelitesChanged( NUMBER_SIGNATURE_SIGNAL );
  void ageOfDifferentialDataChanged( NUMBER_SIGNATURE_SIGNAL );

public Q_SLOTS:
  void setData( const QByteArray& data );

public:
  void parseData();

private:
  QByteArray dataToParse;
};

class NmeaParserGGAFactory : public BlockFactory {
  Q_OBJECT

public:
  NmeaParserGGAFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "NMEA Parser GGA/GNS" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Parsers" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "NMEA Parser for GGA/GNS sentences" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
