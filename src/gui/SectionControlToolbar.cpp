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

#include "SectionControlToolbar.h"
#include "ui_SectionControlToolbar.h"

SectionControlToolbar::SectionControlToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::SectionControlToolbar ) {
  ui->setupUi( this );

  ui->lbName->hide();

  buttonDefault = parent->palette();

  buttonYellow = parent->palette();
  buttonYellow.setColor( QPalette::Button, QColor( 190, 190, 0 ) );

  buttonGreen = parent->palette();
  buttonGreen.setColor( QPalette::Button, QColor( 90, 170, 90 ) );

  buttonRed = parent->palette();
  buttonRed.setColor( QPalette::Button, QColor( 255, 150, 150 ) );

  showHideButtons();
}

SectionControlToolbar::~SectionControlToolbar() {
  delete ui;
}

void SectionControlToolbar::setNumberOfSections( float number ) {
  if( number > 14 ) {
    number = 14;
  }

  numberOfSections = int8_t( number );

  showHideButtons();
}

void SectionControlToolbar::setName( const QString& name ) {
  if( !name.isEmpty() ) {
    ui->lbName->setText( name );
    ui->lbName->show();
  } else {
    ui->lbName->hide();
  }
}

void SectionControlToolbar::showHideButtons() {

  if( numberOfSections >= 14 ) {
    ui->pb14->show();
  } else {
    ui->pb14->hide();
  }

  if( numberOfSections >= 13 ) {
    ui->pb13->show();
  } else {
    ui->pb13->hide();
  }

  if( numberOfSections >= 12 ) {
    ui->pb12->show();
  } else {
    ui->pb12->hide();
  }

  if( numberOfSections >= 11 ) {
    ui->pb11->show();
  } else {
    ui->pb11->hide();
  }

  if( numberOfSections >= 10 ) {
    ui->pb10->show();
  } else {
    ui->pb10->hide();
  }

  if( numberOfSections >= 9 ) {
    ui->pb9->show();
  } else {
    ui->pb9->hide();
  }

  if( numberOfSections >= 8 ) {
    ui->pb8->show();
  } else {
    ui->pb8->hide();
  }

  if( numberOfSections >= 7 ) {
    ui->pb7->show();
  } else {
    ui->pb7->hide();
  }

  if( numberOfSections >= 6 ) {
    ui->pb6->show();
  } else {
    ui->pb6->hide();
  }

  if( numberOfSections >= 5 ) {
    ui->pb5->show();
  } else {
    ui->pb5->hide();
  }

  if( numberOfSections >= 4 ) {
    ui->pb4->show();
  } else {
    ui->pb4->hide();
  }

  if( numberOfSections >= 3 ) {
    ui->pb3->show();
  } else {
    ui->pb3->hide();
  }

  if( numberOfSections >= 2 ) {
    ui->pb2->show();
  } else {
    ui->pb2->hide();
  }

  if( numberOfSections >= 1 ) {
    ui->pb1->show();
  } else {
    ui->pb1->hide();
  }
}

void SectionControlToolbar::advanceState( QWidget* button, SectionControlToolbar::ButtonState& state ) {
  switch( state ) {
    case ButtonState::Normal:
      state = ButtonState::ForceOn;
      button->setPalette( buttonGreen );
      break;

    case ButtonState::ForceOn:
      state = ButtonState::ForceOff;
      button->setPalette( buttonRed );
      break;

    default:
      state = ButtonState::Normal;
      button->setPalette( buttonDefault );
      break;
  }
}

void SectionControlToolbar::on_pbAll_clicked() {
  advanceState( ui->pbAll, pbAllState );
}

void SectionControlToolbar::on_pb1_clicked() {
  advanceState( ui->pb1, pb1State );
}

void SectionControlToolbar::on_pb2_clicked() {
  advanceState( ui->pb2, pb2State );
}

void SectionControlToolbar::on_pb3_clicked() {
  advanceState( ui->pb3, pb3State );
}

void SectionControlToolbar::on_pb4_clicked() {
  advanceState( ui->pb4, pb4State );
}

void SectionControlToolbar::on_pb5_clicked() {
  advanceState( ui->pb5, pb5State );
}

void SectionControlToolbar::on_pb6_clicked() {
  advanceState( ui->pb6, pb6State );
}

void SectionControlToolbar::on_pb7_clicked() {
  advanceState( ui->pb7, pb7State );
}

void SectionControlToolbar::on_pb8_clicked() {
  advanceState( ui->pb8, pb8State );
}

void SectionControlToolbar::on_pb9_clicked() {
  advanceState( ui->pb9, pb9State );
}

void SectionControlToolbar::on_pb10_clicked() {
  advanceState( ui->pb10, pb10State );
}

void SectionControlToolbar::on_pb11_clicked() {
  advanceState( ui->pb11, pb11State );
}

void SectionControlToolbar::on_pb12_clicked() {
  advanceState( ui->pb12, pb12State );
}

void SectionControlToolbar::on_pb13_clicked() {
  advanceState( ui->pb13, pb13State );
}

void SectionControlToolbar::on_pb14_clicked() {
  advanceState( ui->pb14, pb14State );
}
