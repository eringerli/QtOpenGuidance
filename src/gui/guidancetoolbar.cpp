#include "guidancetoolbar.h"
#include "ui_guidancetoolbar.h"

GuidanceToolbar::GuidanceToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::GuidanceToolbar ) {
  ui->setupUi( this );
}

GuidanceToolbar::~GuidanceToolbar() {
  delete ui;
}

void GuidanceToolbar::on_checkBox_stateChanged( int arg1 ) {
  bool enabled = false;

  if( arg1 == Qt::Checked ) {
    enabled = true;
  }

  emit simulatorChanged( enabled );
}

void GuidanceToolbar::on_btn_settings_clicked() {
  emit toggleSettings();
}
