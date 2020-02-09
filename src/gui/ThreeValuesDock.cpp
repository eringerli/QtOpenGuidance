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

#include "ThreeValuesDock.h"
#include "ui_ThreeValuesDock.h"

#include <QVector3D>

#include <QtMath>

#include <cmath>

ThreeValuesDock::ThreeValuesDock( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::ThreeValuesDock ) {
  ui->setupUi( this );
}

ThreeValuesDock::~ThreeValuesDock() {
  delete ui;
}

void ThreeValuesDock::setName( const QString& name ) {
  ui->lbName->setText( name );
}

void ThreeValuesDock::setCaptionEnabled( bool enabled ) {
  ui->lbName->setVisible( enabled );
  ui->lbName->setEnabled( enabled );
}

void ThreeValuesDock::setFontOfLabel( const QFont& font ) {
  ui->lbFirst->setFont( font );
  ui->lbSecond->setFont( font );
  ui->lbThird->setFont( font );
  ui->lbFirstDesc->setFont( font );
  ui->lbSecondDesc->setFont( font );
  ui->lbThirdDesc->setFont( font );
}

void ThreeValuesDock::setDescriptions( const QString& first, const QString& second, const QString& third ) {
  ui->lbFirstDesc->setText( first );
  ui->lbSecondDesc->setText( second );
  ui->lbThirdDesc->setText( third );
}

void ThreeValuesDock::setValues( const double first, const double second, const double third ) {
  double values[] = {double( first ), double( second ), double( third )};

  for( double& val : values ) {
    if( ( val > 0 ) && ( ( val ) < ( 1 / qPow( 10, precision ) ) ) ) {
      val = 0;
    }

    if( ( val < 0 ) && ( ( -val ) < ( 1 / qPow( 10, precision ) ) ) ) {
      val = 0;
    }

    val *= scale;
  }

  ui->lbFirst->setText( QStringLiteral( "%1" ).arg( values[0], fieldWidth, 'f', precision, ' ' ) );
  ui->lbSecond->setText( QStringLiteral( "%1" ).arg( values[1], fieldWidth, 'f', precision, ' ' ) );
  ui->lbThird->setText( QStringLiteral( "%1" ).arg( values[2], fieldWidth, 'f', precision, ' ' ) );
}

const QFont& ThreeValuesDock::fontOfLabel() {
  return ui->lbFirst->font();
}

bool ThreeValuesDock::captionEnabled() {
  return ui->lbName->isVisible();
}
