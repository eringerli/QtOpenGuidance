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

  if( ui->cbAlphaShape->currentText() == QLatin1String( "Optimal" ) ) {
    ui->dsbAlpha->setValue( optimalAlpha );
  }

  if( ui->cbAlphaShape->currentText() == QLatin1String( "Solid" ) ) {
    ui->dsbAlpha->setValue( solidAlpha );
  }
}

void FieldsOptimitionToolbar::setFieldStatistics( double pointsRecorded, double pointsCalculated, double pointsInPolygon ) {
  ui->lbPointsRecorded->setText( QString::number( pointsRecorded ) + " | " + QString::number( pointsCalculated ) );
  ui->lbPointsInPolygon->setNum( pointsInPolygon );
}

void FieldsOptimitionToolbar::on_pbRecalculate_clicked() {
  emit recalculate( AlphaType( ui->cbAlphaShape->currentIndex() ),
                    ui->dsbAlpha->value(),
                    ui->dsbMaxDeviation->value(),
                    ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

void FieldsOptimitionToolbar::on_cbAlphaShape_currentTextChanged( const QString& arg1 ) {
  ui->dsbAlpha->setEnabled( arg1 == QLatin1String( "Custom" ) );

  if( arg1 == QLatin1String( "Optimal" ) ) {
    ui->dsbAlpha->setValue( optimalAlpha );
  }

  if( arg1 == QLatin1String( "Solid" ) ) {
    ui->dsbAlpha->setValue( solidAlpha );
  }
}
