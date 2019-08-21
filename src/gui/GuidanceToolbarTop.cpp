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

#include <QtMath>
#include <QDebug>

#include <math.h>

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

void GuidanceToolbarTop::setXte( float xte ) {
  float xteAbsInCm = qAbs( xte * 100 );
  ui->lbXte->setNum( int( xteAbsInCm ) );

  if( xteAbsInCm > 100 ) {
    ui->pbXteLeft->setStyleSheet( QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                                  "QProgressBar::chunk { background-color: red; width: 1px; }" ) );
    ui->pbXteRight->setStyleSheet( QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                                   "QProgressBar::chunk { background-color: red; width: 1px; }" ) );
  } else if( xteAbsInCm > 20 ) {
    ui->pbXteLeft->setStyleSheet( QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                                  "QProgressBar::chunk { background-color: orange; width: 1px; }" ) );
    ui->pbXteRight->setStyleSheet( QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                                   "QProgressBar::chunk { background-color: orange; width: 1px; }" ) );
  } else {
    ui->pbXteLeft->setStyleSheet( QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                                  "QProgressBar::chunk { background-color: green; width: 1px; }" ) );
    ui->pbXteRight->setStyleSheet( QStringLiteral( "QProgressBar { margin: 0px; padding: 0px; border: 0px solid grey; background-color: #eff0f1; } "
                                   "QProgressBar::chunk { background-color: green; width: 1px; }" ) );
  }

  if( qIsNull( xte ) ) {
    ui->pbXteLeft->setValue( 0 );
    ui->pbXteRight->setValue( 0 );
  } else if( xte > 0 ) {
    ui->pbXteLeft->setValue( int( log10( xteAbsInCm ) * 700 ) );
    ui->pbXteRight->setValue( 0 );
  } else {
    ui->pbXteLeft->setValue( 0 );
    ui->pbXteRight->setValue( int( log10( xteAbsInCm ) * 700 ) );
  }
}
