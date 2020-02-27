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


#include "BlockBase.h"

class NmeaParserRMC : public BlockBase {
    Q_OBJECT

  public:
    explicit NmeaParserRMC()
      : BlockBase() {
    }

  signals:
    void globalPositionChanged( const double, const double, const double );
    void velocityChanged( const double );

  public slots:
    void setData( const QByteArray& data ) {
      dataToParse.append( data );
      parseData();
    }

  public:
    void parseData() {
      // relies heavy on the implementations of QTextStream, QByteArray and QString
      // read the docs about them!

      // create a qtextstream out of dataToParse. dataToParse is not altered by it!
      QTextStream textstream( dataToParse, QIODevice::ReadOnly );

      QString currentLine;
      bool hasChecksum = false;
      quint8 checksumFromNmeaSentence = 0;

      // returns true if a line could be read; the newline has to be there or it returns false
      // currentLine has no trailing end-of-line characters
      if( textstream.readLineInto( &currentLine ) ) {

        // calculate the checksum
        quint8 checksum = 0;

        // i=1; skip the first character for checksum generation
        for( int i = 1; i < currentLine.size(); ++i ) {
          char c = currentLine.at( i ).toLatin1();

          // checksum handling
          if( c == '*' ) {
            // only read out an existing checksum
            if( ( i + 3 ) <= currentLine.size() ) {
              // read the next two chars from the string and convert from hex to quint8
              QString checksumOfCurrentLine;
              checksumOfCurrentLine.append( currentLine.at( i + 1 ) );
              checksumOfCurrentLine.append( currentLine.at( i + 2 ) );
              checksumFromNmeaSentence = quint8( checksumOfCurrentLine.toUInt( nullptr, 16 ) );

              hasChecksum = true;
            }

            // remove all chars behind the * from the string; works with an empty checksum too
            currentLine.remove( i, currentLine.count() - i );

            break;
          } else {
            // the checksum is a simple XOR of all chars in the sentence, but without the $ and the checksum itself
            checksum ^= c;
          }
        }

        QStringList nmeaFields = currentLine.split( ',' );

        // cut off the first three characters: these are '$' and the talker id
        nmeaFields.first().remove( 0, 3 );

        // check the checksum
        if( hasChecksum ) {
          if( checksum == checksumFromNmeaSentence ) {
//            qDebug() << "Checksum correct.";
          } else {
            qDebug() << "Checksum incorrect!";
          }

          // as most users will have either a M8T or F9P, the interface documentation of ublox
          // is used for the format of the NMEA sentences
          // https://www.u-blox.com/de/product/zed-f9p-module -> interface manual
          if( nmeaFields.front() == QStringLiteral( "RMC" ) ) {

            if( nmeaFields.count() >= 12 ) {
              qDebug() << nmeaFields;

              QStringList::const_iterator nmeaFileIterator = nmeaFields.cbegin();
              // skip first field
              ++nmeaFileIterator;

              // skip UTC time
              ++nmeaFileIterator;

              // skip status
              ++nmeaFileIterator;

              // the format is like this: DDMM.MMMMM
              double latitude = nmeaFileIterator->leftRef( 2 ).toDouble();
              latitude += nmeaFileIterator->midRef( 2, 2 ).toDouble() / 60;
              latitude += nmeaFileIterator->midRef( 4 ).toDouble() / 60;
              ++nmeaFileIterator;

              if( ( *nmeaFileIterator ) == 'S' ) {
                latitude = -latitude;
              }

              ++nmeaFileIterator;

              // the format is like this: DDDMM.MMMMM
              double longitude = nmeaFileIterator->leftRef( 3 ).toDouble();
              longitude += nmeaFileIterator->midRef( 3, 2 ).toDouble() / 60;
              longitude += nmeaFileIterator->midRef( 5 ).toDouble() / 60;
              ++nmeaFileIterator;

              if( ( *nmeaFileIterator ) == 'W' ) {
                longitude = -longitude;
              }

              ++nmeaFileIterator;

              // speed in kn: 1kn = 463m/900s
              double velocity = nmeaFileIterator->leftRef( 2 ).toDouble();
              velocity += nmeaFileIterator->midRef( 2 ).toDouble() / 10;
              velocity *= 463;
              velocity /= 900;

              emit velocityChanged( float( velocity ) );
              emit globalPositionChanged( latitude, longitude, 0 );
            }
          }
        }

        // remove the line from the buffer
        dataToParse.remove( 0, int( textstream.pos() ) );
      }
    }

  public:


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

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new NmeaParserRMC();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ) );
      b->addOutputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SIGNAL( globalPositionChanged( const double, const double, const double ) ) ) );
      b->addOutputPort( QStringLiteral( "Velocity" ), QLatin1String( SIGNAL( velocityChanged( const double ) ) ) );

      return b;
    }
};

