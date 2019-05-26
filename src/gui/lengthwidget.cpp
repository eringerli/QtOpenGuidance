#include "lengthwidget.h"
#include "ui_lengthwidget.h"

LengthWidget::LengthWidget( QWidget* parent ) :
  QWidget( parent ),
  ui( new Ui::LengthWidget ) {
  ui->setupUi( this );
}

LengthWidget::~LengthWidget() {
  delete ui;
}

void LengthWidget::on_doubleSpinBox_valueChanged( double arg1 ) {
  emit lengthChanged( ( float( arg1 ) ) );
}
