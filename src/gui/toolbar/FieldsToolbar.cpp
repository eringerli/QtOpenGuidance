// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QBoxLayout>

#include "FieldsToolbar.h"
#include "ui_FieldsToolbar.h"

FieldsToolbar::FieldsToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::FieldsToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  QObject::connect( ui->pbRecordContinous, &QAbstractButton::toggled, this, &FieldsToolbar::continousRecordToggled );
  QObject::connect( ui->pbRecordPoint, &QAbstractButton::clicked, this, &FieldsToolbar::recordPoint );

  QObject::connect( ui->pbEdgeOfImplement, &QAbstractButton::toggled, this, &FieldsToolbar::recordOnEdgeOfImplementChanged );
}

FieldsToolbar::~FieldsToolbar() { delete ui; }

void
FieldsToolbar::on_pbEdgeOfImplement_clicked( bool checked ) {
  if( checked ) {
    ui->pbEdgeOfImplement->setText( "⟶|" );
  } else {
    ui->pbEdgeOfImplement->setText( "|⟵" );
  }
}
