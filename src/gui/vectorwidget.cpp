#include "vectorwidget.h"
#include "ui_vectorwidget.h"

VectorWidget::VectorWidget( QWidget* parent ) :
  QWidget( parent ),
  ui( new Ui::VectorWidget ), vector() {
  ui->setupUi( this );


  resize( ui->formLayout->sizeHint() );
}

VectorWidget::~VectorWidget() {
  delete ui;
}

void VectorWidget::on_sbX_valueChanged( double arg1 ) {
  vector.setX( ( float )arg1 );
  emit vectorChanged( vector );
}

void VectorWidget::on_sbY_valueChanged( double arg1 ) {
  vector.setY( ( float )arg1 );
  emit vectorChanged( vector );
}

void VectorWidget::on_sbZ_valueChanged( double arg1 ) {
  vector.setZ( ( float )arg1 );
  emit vectorChanged( vector );
}
