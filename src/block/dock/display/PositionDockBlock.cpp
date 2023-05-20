// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PositionDockBlock.h"

#include <QAction>
#include <QBrush>
#include <QMenu>

#include "gui/MyMainWindow.h"
#include "gui/dock/ThreeValuesDock.h"

#include "qneblock.h"
#include "qneport.h"

PositionDockBlock::PositionDockBlock( const QString& uniqueName, MyMainWindow* mainWindow ) : ValueDockBlockBase( uniqueName ) {
  widget = new ThreeValuesDock( mainWindow );

  widget->setDescriptions( QStringLiteral( "X" ), QStringLiteral( "Y" ), QStringLiteral( "Z" ) );
}

PositionDockBlock::~PositionDockBlock() {
  widget->deleteLater();

  if( ValueDockBlockBase::firstThreeValuesDock == dock ) {
    ValueDockBlockBase::firstThreeValuesDock = nullptr;
  }
}

const QFont&
PositionDockBlock::getFont() {
  return widget->fontOfLabel();
}

int
PositionDockBlock::getPrecision() {
  return widget->precision;
}

int
PositionDockBlock::getFieldWidth() {
  return widget->fieldWidth;
}

double
PositionDockBlock::getScale() {
  return widget->scale;
}

bool
PositionDockBlock::unitVisible() {
  return widget->unitEnabled;
}

const QString&
PositionDockBlock::getUnit() {
  return widget->unit;
}

void
PositionDockBlock::setFont( const QFont& font ) {
  widget->setFontOfLabel( font );
}

void
PositionDockBlock::setPrecision( const int precision ) {
  widget->precision = precision;
}

void
PositionDockBlock::setFieldWidth( const int fieldWidth ) {
  widget->fieldWidth = fieldWidth;
}

void
PositionDockBlock::setScale( const double scale ) {
  widget->scale = scale;
}

void
PositionDockBlock::setUnitVisible( const bool enabled ) {
  widget->unitEnabled = enabled;
}

void
PositionDockBlock::setUnit( const QString& unit ) {
  widget->unit = unit;
}

void
PositionDockBlock::setName( const QString& name ) {
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "Position: " ) + name );
}

void
PositionDockBlock::setPose( const Eigen::Vector3d& point, const Eigen::Quaterniond&, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::ValueDock ) ) {
    widget->setDescriptions( QStringLiteral( "X" ), QStringLiteral( "Y" ), QStringLiteral( "Z" ) );

    widget->setValues( point.x(), point.y(), point.z() );
  }
}

void
PositionDockBlock::setVector( const Eigen::Vector3d& position, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::ValueDock ) ) {
    widget->setDescriptions( QStringLiteral( "X" ), QStringLiteral( "Y" ), QStringLiteral( "Z" ) );

    widget->setValues( position.x(), position.y(), position.z() );
  }
}

void
PositionDockBlock::setWgs84( const Eigen::Vector3d& position, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::ValueDock ) ) {
    widget->setDescriptions( QStringLiteral( "Lon" ), QStringLiteral( "Lat" ), QStringLiteral( "H" ) );

    widget->setValues( position.x(), position.y(), position.z() );
  }
}

QNEBlock*
PositionDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* obj = new PositionDockBlock( getNameOfFactory() + QString::number( id ), mainWindow );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( ValueDockBlockBase::firstThreeValuesDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    ValueDockBlockBase::firstThreeValuesDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, ValueDockBlockBase::firstThreeValuesDock );
  }

  b->addInputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SLOT( setWgs84( VECTOR_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Vector" ), QLatin1String( SLOT( setVector( VECTOR_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  b->setBrush( dockColor );

  return b;
}
