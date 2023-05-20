// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XteDock.h"
#include "ui_XteDock.h"

#include <cmath>

XteDock::XteDock( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::XteDock ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  setXte( 0. );
}

XteDock::~XteDock() { delete ui; }

void
XteDock::setXte( double xte ) {
  auto xteAbsInCm = xte * 100.0;
  auto meter      = std::abs( xte * scale );

  if( ( meter > 0 ) && ( ( meter ) < ( 1 / std::pow( 10, precision ) ) ) ) {
    meter = 0;
  }

  if( std::isinf( xte ) ) {
    ui->lbXte->setText( QStringLiteral( "Inf" ) );
    ui->pbXteLeft->setValue( 0 );
    ui->pbXteRight->setValue( 0 );
  } else {
    ui->lbXte->setText( QStringLiteral( "%1" ).arg( meter, fieldWidth, 'f', precision, ' ' ) );

    if( unitEnabled && !unit.isEmpty() ) {
      ui->lbUnit->setVisible( true );
      ui->lbUnit->setText( unit );
    } else {
      ui->lbUnit->setVisible( false );
    }

    if( xteAbsInCm > 100 ) {
      ui->pbXteLeft->setStyleSheet(
        QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                        "QProgressBar::chunk { background-color: red; width: 1px; }" ) );
      ui->pbXteRight->setStyleSheet(
        QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                        "QProgressBar::chunk { background-color: red; width: 1px; }" ) );
    } else if( xteAbsInCm > 20 ) {
      ui->pbXteLeft->setStyleSheet(
        QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                        "QProgressBar::chunk { background-color: orange; width: 1px; }" ) );
      ui->pbXteRight->setStyleSheet(
        QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                        "QProgressBar::chunk { background-color: orange; width: 1px; }" ) );
    } else {
      ui->pbXteLeft->setStyleSheet(
        QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                        "QProgressBar::chunk { background-color: green; width: 1px; }" ) );
      ui->pbXteRight->setStyleSheet(
        QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                        "QProgressBar::chunk { background-color: green; width: 1px; }" ) );
    }
  }

  if( qIsNull( xte ) ) {
    ui->pbXteLeft->setValue( 0 );
    ui->pbXteRight->setValue( 0 );
  } else if( xte > 0 ) {
    ui->pbXteLeft->setValue( int( std::log10( meter ) * 700 ) );
    ui->pbXteRight->setValue( 0 );
  } else {
    ui->pbXteLeft->setValue( 0 );
    ui->pbXteRight->setValue( int( std::log10( meter ) * 700 ) );
  }
}

void
XteDock::setName( const QString& name ) {
  ui->lbName->setText( name );
}

void
XteDock::setFontOfLabel( const QFont& font ) {
  ui->lbXte->setFont( font );
}

const QFont&
XteDock::fontOfLabel() {
  return ui->lbXte->font();
}
