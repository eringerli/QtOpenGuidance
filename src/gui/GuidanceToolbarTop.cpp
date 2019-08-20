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

  if( xte > 0 ) {
    ui->pbXteLeft->setValue( int( log10( xteAbsInCm ) * 700 ) );
    ui->pbXteRight->setValue( 0 );
  } else {
    ui->pbXteLeft->setValue( 0 );
    ui->pbXteRight->setValue( int( log10( xteAbsInCm ) * 700 ) );
  }
}
