#include "PassToolbar.h"
#include "ui_PassToolbar.h"

PassToolbar::PassToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::PassToolbar ) {
  ui->setupUi( this );
}

PassToolbar::~PassToolbar() {
  delete ui;
}
