// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ThreeValuesDock.h"
#include "ui_ThreeValuesDock.h"

#include <QVector3D>

#include <QtMath>

#include <cmath>

ThreeValuesDock::ThreeValuesDock( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::ThreeValuesDock ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

ThreeValuesDock::~ThreeValuesDock() { delete ui; }

void
ThreeValuesDock::setFontOfLabel( const QFont& font ) {
  ui->lbFirst->setFont( font );
  ui->lbSecond->setFont( font );
  ui->lbThird->setFont( font );
  ui->lbFirstDesc->setFont( font );
  ui->lbSecondDesc->setFont( font );
  ui->lbThirdDesc->setFont( font );
}

void
ThreeValuesDock::setDescriptions( const QString& first, const QString& second, const QString& third ) {
  ui->lbFirstDesc->setText( first );
  ui->lbSecondDesc->setText( second );
  ui->lbThirdDesc->setText( third );
}

void
ThreeValuesDock::setValues( const double first, const double second, const double third ) {
  double values[] = { double( first ), double( second ), double( third ) };

  for( double& val : values ) {
    if( ( val > 0 ) && ( ( val ) < ( 1 / qPow( 10, precision ) ) ) ) {
      val = 0;
    }

    if( ( val < 0 ) && ( ( -val ) < ( 1 / qPow( 10, precision ) ) ) ) {
      val = 0;
    }

    val *= scale;
  }

  if( unitEnabled ) {
    ui->lbFirst->setText( QStringLiteral( "%1%2" ).arg( values[0], fieldWidth, 'f', precision, ' ' ).arg( unit ) );
    ui->lbSecond->setText( QStringLiteral( "%1%2" ).arg( values[1], fieldWidth, 'f', precision, ' ' ).arg( unit ) );
    ui->lbThird->setText( QStringLiteral( "%1%2" ).arg( values[2], fieldWidth, 'f', precision, ' ' ).arg( unit ) );
  } else {
    ui->lbFirst->setText( QStringLiteral( "%1" ).arg( values[0], fieldWidth, 'f', precision, ' ' ) );
    ui->lbSecond->setText( QStringLiteral( "%1" ).arg( values[1], fieldWidth, 'f', precision, ' ' ) );
    ui->lbThird->setText( QStringLiteral( "%1" ).arg( values[2], fieldWidth, 'f', precision, ' ' ) );
  }
}

const QFont&
ThreeValuesDock::fontOfLabel() {
  return ui->lbFirst->font();
}
