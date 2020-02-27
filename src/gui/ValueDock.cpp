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

#include "ValueDock.h"
#include "ui_ValueDock.h"

#include <QtMath>

#include <cmath>

ValueDock::ValueDock( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::ValueDock ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

ValueDock::~ValueDock() {
  delete ui;
}

void ValueDock::setMeter( double arg ) {
  auto meter = double( arg );
  meter *= scale;

  if( ( meter > 0 ) && ( ( meter ) < ( 1 / qPow( 10, precision ) ) ) ) {
    meter = 0;
  }

  if( ( meter < 0 ) && ( ( -meter ) < ( 1 / qPow( 10, precision ) ) ) ) {
    meter = 0;
  }

  ui->lbMeter->setText( QStringLiteral( "%1" ).arg( meter, fieldWidth, 'f', precision, ' ' ) );
}

void ValueDock::setName( const QString& name ) {
  ui->lbName->setText( name );
}

void ValueDock::setCaptionEnabled( bool enabled ) {
  ui->lbName->setVisible( enabled );
  ui->lbName->setEnabled( enabled );
}

void ValueDock::setFontOfLabel( const QFont& font ) {
  ui->lbMeter->setFont( font );
}

const QFont& ValueDock::fontOfLabel() {
  return ui->lbMeter->font();
}

bool ValueDock::captionEnabled() {
  return ui->lbName->isVisible();
}

#include "moc_ValueDock.cpp"
