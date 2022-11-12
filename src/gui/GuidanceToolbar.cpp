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

#include <QMenu>
#include <QSettings>
#include <QStandardPaths>

#include "GuidanceToolbar.h"
#include "ui_GuidanceToolbar.h"

GuidanceToolbar::GuidanceToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::GuidanceToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  menu = new QMenu( this );

  ui->pbDocks->setMenu( menu );
  ui->pbDocks->setPopupMode( QToolButton::InstantPopup );
}

GuidanceToolbar::~GuidanceToolbar() { delete ui; }

void
GuidanceToolbar::on_cbSimulator_stateChanged( int arg1 ) {
  bool enabled = false;

  if( arg1 == Qt::Checked ) {
    enabled = true;
  }

  Q_EMIT simulatorChanged( enabled );
}

void
GuidanceToolbar::on_pbSettings_clicked() {
  Q_EMIT toggleSettings();
}

void
GuidanceToolbar::cbSimulatorSetChecked( bool enabled ) {
  ui->cbSimulator->setChecked( enabled );
}
