#include <QBoxLayout>

#include "FieldsToolbar.h"
#include "ui_FieldsToolbar.h"

FieldsToolbar::FieldsToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::FieldsToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  boxLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
  boxLayout->addWidget( ui->pbOpen );
  boxLayout->addWidget( ui->pbNew );
  boxLayout->addWidget( ui->pbSave );

  boxLayout->addStretch( 1 );

  boxLayout->addWidget( ui->pbRecordContinous );
  boxLayout->addWidget( ui->pbRecordPoint );
  boxLayout->addWidget( ui->pbEdgeOfImplement );

  this->setLayout( boxLayout );

  QObject::connect( ui->pbOpen, &QAbstractButton::clicked, this, &FieldsToolbar::openField );
  QObject::connect( ui->pbNew, &QAbstractButton::clicked, this, &FieldsToolbar::newField );
  QObject::connect( ui->pbSave, &QAbstractButton::clicked, this, &FieldsToolbar::saveField );

  QObject::connect( ui->pbRecordContinous, &QAbstractButton::toggled, this, &FieldsToolbar::continousRecordToggled );
  QObject::connect( ui->pbRecordPoint, &QAbstractButton::clicked, this, &FieldsToolbar::recordPoint );

  QObject::connect( ui->pbEdgeOfImplement, &QAbstractButton::toggled, this, &FieldsToolbar::recordOnEdgeOfImplementChanged );
}

FieldsToolbar::~FieldsToolbar() {
  delete ui;
}

void FieldsToolbar::setDockLocation( Qt::DockWidgetArea area ) {
  if( area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea ) {
    boxLayout->setDirection( QBoxLayout::TopToBottom );
    ui->pbOpen->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    ui->pbNew->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    ui->pbSave->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    ui->pbRecordContinous->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    ui->pbRecordPoint->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    ui->pbEdgeOfImplement->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  } else if( area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea ) {
    boxLayout->setDirection( QBoxLayout::LeftToRight );
    ui->pbOpen->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
    ui->pbNew->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
    ui->pbSave->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
    ui->pbRecordContinous->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
    ui->pbRecordPoint->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
    ui->pbEdgeOfImplement->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
  }
}

#include "moc_FieldsToolbar.cpp"
