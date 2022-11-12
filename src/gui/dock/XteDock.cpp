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

#include "XteDock.h"
#include "ui_XteDock.h"

#include <cmath>

XteDock::XteDock( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::XteDock ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

XteDock::~XteDock() { delete ui; }

void
XteDock::setXte( double xte ) {
  auto xteAbsInCm = xte * 100.0;
  auto meter      = std::abs( xte * scale );

  if( ( meter > 0 ) && ( ( meter ) < ( 1 / std::pow( 10, precision ) ) ) ) {
    meter = 0;
  }

  if( qIsInf( xte ) ) {
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
