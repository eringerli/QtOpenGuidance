// Copyright( C ) 2019 Christian Riggenbach
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

#include "GuidanceToolbarTop.h"
#include "ui_GuidanceToolbarTop.h"

GuidanceToolbarTop::GuidanceToolbarTop( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::GuidanceToolbarTop ) {
  ui->setupUi( this );
}

GuidanceToolbarTop::~GuidanceToolbarTop() {
  delete ui;
}

void GuidanceToolbarTop::on_pbLeft_clicked() {
  emit turnLeft();
}

void GuidanceToolbarTop::on_pbRight_clicked() {
  emit turnRight();
}
