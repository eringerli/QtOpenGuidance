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

void GuidanceToolbarTop::setXte( double xte ) {
  int xteAbsInCm = qAbs( int( xte * 100 ) );
  ui->lbXte->setNum( xteAbsInCm );

  if( xte > 0 ) {
    ui->pbXteLeft->setValue( int( log10( xteAbsInCm ) * 700 ) );
    ui->pbXteRight->setValue( 0 );
  } else {
    ui->pbXteLeft->setValue( 0 );
    ui->pbXteRight->setValue( int( log10( xteAbsInCm ) * 700 ) );
  }
}
