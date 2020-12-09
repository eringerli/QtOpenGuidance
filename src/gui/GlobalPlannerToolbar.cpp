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

#include "GlobalPlannerToolbar.h"
#include "ui_GlobalPlannerToolbar.h"

GlobalPlannerToolbar::GlobalPlannerToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::GlobalPlannerToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

GlobalPlannerToolbar::~GlobalPlannerToolbar() {
  delete ui;
}

void GlobalPlannerToolbar::on_pbAB_clicked( bool checked ) {
  if( checked ) {
    ui->pbAB->setText( QStringLiteral( "B" ) );
    Q_EMIT setAPoint();
  } else {
    if( ui->pbAB->text() == QStringLiteral( "B" ) ) {
      ui->pbAB->setText( QStringLiteral( "+" ) );
      ui->pbAB->setCheckable( false );
      Q_EMIT setBPoint();
    } else {
      Q_EMIT setAdditionalPoint();
    }
  }
}

void GlobalPlannerToolbar::on_pbSnap_clicked() {
  Q_EMIT snap();
}

void GlobalPlannerToolbar::resetToolbar() {
  ui->pbAB->setText( QStringLiteral( "A" ) );
  ui->pbAB->setCheckable( true );
}

void GlobalPlannerToolbar::setToolbarToAdditionalPoint() {
  ui->pbAB->setText( QStringLiteral( "+" ) );
  ui->pbAB->setCheckable( false );
}

void GlobalPlannerToolbar::on_pbAbContinuous_clicked( bool checked ) {
  Q_EMIT setAdditionalPointsContinous( checked );
}
