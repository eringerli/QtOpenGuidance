// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueDock.h"
#include "ui_ValueDock.h"

#include <QtMath>

#include <cmath>

ValueDock::ValueDock( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::ValueDock ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

ValueDock::~ValueDock() { delete ui; }

void
ValueDock::setMeter( double arg ) {
  auto meter = double( arg );
  meter *= scale;

  if( ( meter > 0 ) && ( ( meter ) < ( 1 / qPow( 10, precision ) ) ) ) {
    meter = 0;
  }

  if( ( meter < 0 ) && ( ( -meter ) < ( 1 / qPow( 10, precision ) ) ) ) {
    meter = 0;
  }

  if( unitEnabled ) {
    ui->lbMeter->setText( QStringLiteral( "%1%2" ).arg( meter, fieldWidth, 'f', precision, ' ' ).arg( unit ) );
  } else {
    ui->lbMeter->setText( QStringLiteral( "%1" ).arg( meter, fieldWidth, 'f', precision, ' ' ) );
  }
}

void
ValueDock::setFontOfLabel( const QFont& font ) {
  ui->lbMeter->setFont( font );
}

const QFont&
ValueDock::fontOfLabel() {
  return ui->lbMeter->font();
}
