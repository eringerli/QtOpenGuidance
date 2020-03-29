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

class NmeaParserHDT : public BlockBase {
    Q_OBJECT

  public:
    explicit NmeaParserHDT()
      : BlockBase() {
    }

  signals:
    void orientationChanged( const QQuaternion& );


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

          // https://www.trimble.com/OEM_ReceiverHelp/V4.44/en/NMEA-0183messages_HDT.html
          if( nmeaFields.front() == QStringLiteral( "HDT" ) ) {
            if( nmeaFields.count() >= 3 ) {
              qDebug() << nmeaFields;

              QStringList::const_iterator nmeaFileIterator = nmeaFields.cbegin();
              // skip first field
              ++nmeaFileIterator;

              emit orientationChanged( QQuaternion::fromAxisAndAngle(
                                               QVector3D( 0.0f, 0.0f, 1.0f ),
                                               nmeaFileIterator->toFloat() ) );
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

class NmeaParserHDTFactory : public BlockFactory {
    Q_OBJECT

  public:
    NmeaParserHDTFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "NMEA Parser HDT" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new NmeaParserHDT();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ) );
      b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( const QQuaternion& ) ) ) );

      b->setBrush( QColor( QStringLiteral( "mediumaquamarine" ) ) );

      return b;
    }
};
