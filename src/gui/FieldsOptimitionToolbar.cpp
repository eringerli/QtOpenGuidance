#include "FieldsOptimitionToolbar.h"
#include "ui_FieldsOptimitionToolbar.h"

FieldsOptimitionToolbar::FieldsOptimitionToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::FieldsOptimitionToolbar ) {
  ui->setupUi( this );
}

FieldsOptimitionToolbar::~FieldsOptimitionToolbar() {
  delete ui;
}

void FieldsOptimitionToolbar::setAlpha( double optimalAlpha, double solidAlpha ) {
  this->optimalAlpha = optimalAlpha;
  this->solidAlpha = solidAlpha;

  if( ui->cbAlphaShape->currentText() == "Optimal" ) {
    ui->dsbAlpha->setValue( optimalAlpha );
  }

  if( ui->cbAlphaShape->currentText() == "Solid" ) {
    ui->dsbAlpha->setValue( solidAlpha );
  }
}

void FieldsOptimitionToolbar::setFieldStatistics( size_t pointsRecorded, size_t pointsInPolygon ) {
  ui->lbPointsInPolygon->setNum( double( pointsInPolygon ) );
  ui->lbPointsRecorded->setNum( double( pointsRecorded ) );
}

void FieldsOptimitionToolbar::on_pbRecalculate_clicked() {
  emit recalculate( AlphaType( ui->cbAlphaShape->currentIndex() ), ui->dsbAlpha->value(), ui->dsbMaxDeviation->value() );
}

void FieldsOptimitionToolbar::on_cbAlphaShape_currentTextChanged( const QString& arg1 ) {
  ui->dsbAlpha->setEnabled( arg1 == "Custom" );

  if( arg1 == "Optimal" ) {
    ui->dsbAlpha->setValue( optimalAlpha );
  }

  if( arg1 == "Solid" ) {
    ui->dsbAlpha->setValue( solidAlpha );
  }
}
