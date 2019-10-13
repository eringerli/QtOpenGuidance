#include "GuidanceMeterBar.h"
#include "ui_GuidanceMeterBar.h"

#include <QtMath>

#include <cmath>

GuidanceMeterBar::GuidanceMeterBar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::GuidanceMeterBar ) {
  ui->setupUi( this );
}

GuidanceMeterBar::~GuidanceMeterBar() {
  delete ui;
}

void GuidanceMeterBar::setMeter( float meter ) {
  meter *= scale;

  if( ( meter > 0 ) && ( ( meter ) < float( 1 / qPow( 10, precision ) ) ) ) {
    meter = 0;
  }

  if( ( meter < 0 ) && ( ( -meter ) < float( 1 / qPow( 10, precision ) ) ) ) {
    meter = 0;
  }

  ui->lbMeter->setText( QStringLiteral( "%1" ).arg( double( meter ), fieldwitdh, 'f', precision, ' ' ) );
}

void GuidanceMeterBar::setPrecision( float precision ) {
  this->precision = int( precision );
}

void GuidanceMeterBar::setScale( float scale ) {
  this->scale = scale;
}

void GuidanceMeterBar::setFieldWitdh( float fieldwitdh ) {
  this->fieldwitdh = int( fieldwitdh );
}

void GuidanceMeterBar::setName( const QString& name ) {
  ui->lbName->setText( name );
}
