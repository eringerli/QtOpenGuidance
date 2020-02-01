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

#include "OrientationDock.h"
#include "ui_OrientationDock.h"

#include <QVector3D>

#include <QtMath>

#include <cmath>

OrientationDock::OrientationDock( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::OrientationDock ) {
  ui->setupUi( this );
}

OrientationDock::~OrientationDock() {
  delete ui;
}

void OrientationDock::setOrientation( QQuaternion quaternion ) {

  QVector3D eulerAngles = quaternion.toEulerAngles();
  eulerAngles *= scale;

  for( int i = 0; i < 3; ++i ) {
    if( ( eulerAngles[i] > 0 ) && ( ( eulerAngles[i] ) < ( 1 / qPow( 10, precision ) ) ) ) {
      eulerAngles[i] = 0;
    }

    if( ( eulerAngles[i] < 0 ) && ( ( -eulerAngles[i] ) < ( 1 / qPow( 10, precision ) ) ) ) {
      eulerAngles[i] = 0;
    }
  }

  ui->lbRoll->setText( QStringLiteral( "%1" ).arg( eulerAngles.y(), fieldWidth, 'f', precision, ' ' ) );
  ui->lbPitch->setText( QStringLiteral( "%1" ).arg( eulerAngles.x(), fieldWidth, 'f', precision, ' ' ) );
  ui->lbHeading->setText( QStringLiteral( "%1" ).arg( eulerAngles.z(), fieldWidth, 'f', precision, ' ' ) );
}

void OrientationDock::setName( const QString& name ) {
  ui->lbName->setText( name );
}

void OrientationDock::setCaptionEnabled( bool enabled ) {
  ui->lbName->setVisible( enabled );
  ui->lbName->setEnabled( enabled );
}

void OrientationDock::setFontOfLabel( const QFont& font ) {
  ui->lbRoll->setFont( font );
  ui->lbPitch->setFont( font );
  ui->lbHeading->setFont( font );
  ui->lbRoll_2->setFont( font );
  ui->lbPitch_2->setFont( font );
  ui->lbHeading_2->setFont( font );
}

const QFont& OrientationDock::fontOfLabel() {
  return ui->lbRoll->font();
}

bool OrientationDock::captionEnabled() {
  return ui->lbName->isVisible();
}
