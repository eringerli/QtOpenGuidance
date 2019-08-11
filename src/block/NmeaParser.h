// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef NMEAPARSER_H
#define NMEAPARSER_H

#include <QObject>


#include "BlockBase.h"

class NmeaParser : public BlockBase {
    Q_OBJECT

  public:
    explicit NmeaParser()
      : BlockBase() {
    }

    void emitConfigSignals() override {
    }

  signals:
    void globalPositionChanged( double latitude, double longitude, double height );

  public slots:
    void setData( QByteArray data ) {
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

          // GGA and GNS are exactly the same, but GNS displays more than 12 satelites (max 99)
          if( nmeaFields.front() == "GGA" || nmeaFields.front() == "GNS" ) {
            if( nmeaFields.count() >= 14 ) {
              qDebug() << nmeaFields;

              QStringList::const_iterator nmeaFileIterator = nmeaFields.cbegin();
              // skip first field
              ++nmeaFileIterator;

              utcTime = nmeaFileIterator->toDouble();
              ++nmeaFileIterator;

              // the format is like this: DDMM.MMMMM
              latitude = nmeaFileIterator->left( 2 ).toDouble();
              latitude += nmeaFileIterator->mid( 2, 2 ).toDouble() / 60;
              latitude += nmeaFileIterator->mid( 4 ).toDouble() / 60;
              ++nmeaFileIterator;

              if( ( *nmeaFileIterator ) == 'S' ) {
                latitude = -latitude;
              }

              ++nmeaFileIterator;

              // the format is like this: DDDMM.MMMMM
              longitude = nmeaFileIterator->left( 3 ).toDouble();
              longitude += nmeaFileIterator->mid( 3, 2 ).toDouble() / 60;
              longitude += nmeaFileIterator->mid( 5 ).toDouble() / 60;
              ++nmeaFileIterator;

              if( ( *nmeaFileIterator ) == 'W' ) {
                longitude = -longitude;
              }

              ++nmeaFileIterator;

              fixQuality = nmeaFileIterator->toDouble();
              ++nmeaFileIterator;

              numSatelites = nmeaFileIterator->toDouble();
              ++nmeaFileIterator;

              hdop = nmeaFileIterator->toDouble();
              ++nmeaFileIterator;

              height = nmeaFileIterator->toDouble();
              ++nmeaFileIterator;

              // skip unit of height
              ++nmeaFileIterator;

              // skip geoid seperation
              ++nmeaFileIterator;

              // skip unit of geoid seperation
              ++nmeaFileIterator;

              ageOfDifferentialData = nmeaFileIterator->toDouble();

              emit globalPositionChanged( latitude, longitude, height );

              qDebug() << qSetRealNumberPrecision( 12 ) << utcTime << latitude << longitude << height << fixQuality << numSatelites << hdop << ageOfDifferentialData;
            }
          } else {
            if( nmeaFields.front() == "RMC" ) {
            } else {
              if( nmeaFields.front() == "GSA" ) {

              }
            }
          }
        }

        // remove the line from the buffer
        dataToParse.remove( 0, int( textstream.pos() ) );
      }
    }

  public:
    double longitude = 0,
           latitude = 0,
           height = 0;

    double utcTime = 0,
           hdop = 0,
           fixQuality = 0,
           ageOfDifferentialData = 0,
           numSatelites = 0;


  private:
    QByteArray dataToParse;
};

class NmeaParserFactory : public BlockFactory {
    Q_OBJECT

  public:
    NmeaParserFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "NMEA Parser" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new NmeaParser();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Data", SLOT( setData( QByteArray ) ) );
      b->addOutputPort( "WGS84 Position", SIGNAL( globalPositionChanged( double, double, double ) ) );

      return b;
    }
};

#endif // NMEAPARSER_H
