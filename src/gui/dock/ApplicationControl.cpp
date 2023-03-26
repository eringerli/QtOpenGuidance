#include "ApplicationControl.h"
#include "qgroupbox.h"
#include "ui_ApplicationControl.h"

ApplicationControl::ApplicationControl( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::ApplicationControl ) { ui->setupUi( this ); }

ApplicationControl::~ApplicationControl() { delete ui; }

void
ApplicationControl::on_pbFullscreen_clicked() {
  Q_EMIT requestFullscreen();
}

void
ApplicationControl::on_pbClose_clicked() {
  Q_EMIT requestClose();
}
