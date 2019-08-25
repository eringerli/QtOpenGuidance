#include "GuidanceXteBar.h"
#include "ui_GuidanceXteBar.h"

#include <math.h>

GuidanceXteBar::GuidanceXteBar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::GuidanceXteBar ) {
  ui->setupUi( this );
}

GuidanceXteBar::~GuidanceXteBar() {
  delete ui;
}

void GuidanceXteBar::setXte( float xte ) {
  float xteAbsInCm = qAbs( xte * 100 );

  if( qIsInf( xte ) ) {
    ui->lbXte->setText( QStringLiteral( "Inf" ) );
    ui->pbXteLeft->setValue( 0 );
    ui->pbXteRight->setValue( 0 );
  } else {
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

void GuidanceXteBar::setName( QString name ) {
  ui->lbName->setText( name );
}
