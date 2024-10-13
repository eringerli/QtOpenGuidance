// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PositionDockBlock.h"

#include <QAction>

#include <QMenu>

#include "gui/MyMainWindow.h"
#include "gui/dock/ThreeValuesDock.h"

PositionDockBlock::PositionDockBlock(
  MyMainWindow* mainWindow, const QString& uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : ValueDockBlockBase( uniqueName, idHint, systemBlock, type ) {
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
PositionDockBlock::getFont() const {
  return widget->fontOfLabel();
}

int
PositionDockBlock::getPrecision() const {
  return widget->precision;
}

int
PositionDockBlock::getFieldWidth() const {
  return widget->fieldWidth;
}

double
PositionDockBlock::getScale() const {
  return widget->scale;
}

bool
PositionDockBlock::unitVisible() const {
  return widget->unitEnabled;
}

const QString&
PositionDockBlock::getUnit() const {
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
  BlockBase::setName( name );

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

std::unique_ptr< BlockBase >
PositionDockBlockFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< PositionDockBlock >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( ValueDockBlockBase::firstThreeValuesDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    ValueDockBlockBase::firstThreeValuesDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, ValueDockBlockBase::firstThreeValuesDock );
  }

  obj->addInputPort( QStringLiteral( "WGS84 Position" ), obj.get(), QLatin1StringView( SLOT( setWgs84( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Vector" ), obj.get(), QLatin1StringView( SLOT( setVector( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  return obj;
}
