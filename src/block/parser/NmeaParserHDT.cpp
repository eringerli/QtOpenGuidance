// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "NmeaParserHDT.h"

#include "kinematic/CalculationOptions.h"
#include "qneblock.h"
#include "qneport.h"

#include <QBrush>
#include <QIODevice>

#include "helpers/anglesHelper.h"

void
NmeaParserHDT::setData( const QByteArray& data ) {
  dataToParse.append( data );
  parseData();
}

void
NmeaParserHDT::parseData() {
  // relies heavy on the implementations of QTextStream, QByteArray and QString
  // read the docs about them!

  // create a qtextstream out of dataToParse. dataToParse is not altered by it!
  QTextStream textstream( dataToParse, QIODevice::ReadOnly );

  QString currentLine;
  bool    hasChecksum              = false;
  quint8  checksumFromNmeaSentence = 0;

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
        currentLine.remove( i, currentLine.size() - i );

        break;
      } else {
        // the checksum is a simple XOR of all chars in the sentence, but without the $
        // and the checksum itself
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

          Q_EMIT orientationChanged(
            Eigen::Quaterniond( Eigen::AngleAxisd( degreesToRadians( nmeaFileIterator->toDouble() ), Eigen::Vector3d::UnitZ() ) ),
            CalculationOption::Option::None );
        }
      }
    }

    // remove the line from the buffer
    dataToParse.remove( 0, int( textstream.pos() ) );
  }
}

QNEBlock*
NmeaParserHDTFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new NmeaParserHDT();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ) );
  b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE ) ) ) );

  b->setBrush( parserColor );

  return b;
}
