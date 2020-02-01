#include "ValueDock.h"
#include "ui_ValueDock.h"

#include <QtMath>

#include <cmath>

ValueDock::ValueDock( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::ValueDock ) {
  ui->setupUi( this );
}

ValueDock::~ValueDock() {
  delete ui;
}

void ValueDock::setMeter( float arg ) {
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

void ValueDock::setName( const QString& name ) {
  ui->lbName->setText( name );
}

void ValueDock::setCaptionEnabled( bool enabled ) {
  ui->lbName->setVisible( enabled );
  ui->lbName->setEnabled( enabled );
}

void ValueDock::setFontOfLabel( const QFont& font ) {
  ui->lbMeter->setFont( font );
}

const QFont& ValueDock::fontOfLabel() {
  return ui->lbMeter->font();
}

bool ValueDock::captionEnabled() {
  return ui->lbName->isVisible();
}
