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

void GuidanceMeterBar::setMeter( float arg ) {
  auto meter = double( arg );
  meter *= scale;

  if( ( meter > 0 ) && ( ( meter ) < ( 1 / qPow( 10, precision ) ) ) ) {
    meter = 0;
  }

  if( ( meter < 0 ) && ( ( -meter ) < ( 1 / qPow( 10, precision ) ) ) ) {
    meter = 0;
  }

  ui->lbMeter->setText( QStringLiteral( "%1" ).arg( meter, fieldWidth, 'f', precision, ' ' ) );
}

void GuidanceMeterBar::setName( const QString& name ) {
  ui->lbName->setText( name );
}

void GuidanceMeterBar::setCaptionEnabled( bool enabled ) {
  ui->lbName->setVisible( enabled );
  ui->lbName->setEnabled( enabled );
}

void GuidanceMeterBar::setFontOfLabel( const QFont& font ) {
  ui->lbMeter->setFont( font );
}

const QFont& GuidanceMeterBar::fontOfLabel() {
  return ui->lbMeter->font();
}

bool GuidanceMeterBar::captionEnabled() {
  return ui->lbName->isVisible();
}
