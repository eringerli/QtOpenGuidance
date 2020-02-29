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

#include "GuidanceTurning.h"
#include "ui_GuidanceTurning.h"

GuidanceTurning::GuidanceTurning( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::GuidanceTurning ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

GuidanceTurning::~GuidanceTurning() {
  delete ui;
}

void GuidanceTurning::on_pbLeft_clicked() {
  emit turnLeft();
}

void GuidanceTurning::on_pbRight_clicked() {
  emit turnRight();
}
