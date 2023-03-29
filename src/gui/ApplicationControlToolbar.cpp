#include "ApplicationControlToolbar.h"
#include "qgroupbox.h"
#include "ui_ApplicationControlToolbar.h"

ApplicationControlToolbar::ApplicationControlToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::ApplicationControlToolbar ) {
  ui->setupUi( this );
}

ApplicationControlToolbar::~ApplicationControlToolbar() { delete ui; }

void
ApplicationControlToolbar::on_pbFullscreen_clicked() {
  Q_EMIT requestFullscreen();
}

void
ApplicationControlToolbar::on_pbClose_clicked() {
  Q_EMIT requestClose();
}
