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

#include <QBoxLayout>

#include "FieldsToolbar.h"
#include "ui_FieldsToolbar.h"

FieldsToolbar::FieldsToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::FieldsToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  QObject::connect( ui->pbRecordContinous, &QAbstractButton::toggled, this, &FieldsToolbar::continousRecordToggled );
  QObject::connect( ui->pbRecordPoint, &QAbstractButton::clicked, this, &FieldsToolbar::recordPoint );

  QObject::connect( ui->pbEdgeOfImplement, &QAbstractButton::toggled, this, &FieldsToolbar::recordOnEdgeOfImplementChanged );
}

FieldsToolbar::~FieldsToolbar() { delete ui; }
