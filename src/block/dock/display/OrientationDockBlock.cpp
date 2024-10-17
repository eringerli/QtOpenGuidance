// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OrientationDockBlock.h"

#include <QAction>

#include <QMenu>

#include "gui/MyMainWindow.h"
#include "gui/dock/ThreeValuesDock.h"

OrientationDockBlock::OrientationDockBlock( MyMainWindow*              mainWindow,
                                            const QString&             uniqueName,
                                            const BlockBaseId          idHint,
                                            const bool                 systemBlock,
                                            const QString              type,
                                            const BlockBase::TypeColor typeColor )
    : ValueDockBlockBase( uniqueName, idHint, systemBlock, type, typeColor ) {
  widget = new ThreeValuesDock( mainWindow );

  widget->setDescriptions( QStringLiteral( "Y" ), QStringLiteral( "P" ), QStringLiteral( "R" ) );
}

OrientationDockBlock::~OrientationDockBlock() {
  widget->deleteLater();

  if( ValueDockBlockBase::firstThreeValuesDock == dock ) {
    ValueDockBlockBase::firstThreeValuesDock = nullptr;
  }
}

const QFont&
OrientationDockBlock::getFont() const {
  return widget->fontOfLabel();
}

int
OrientationDockBlock::getPrecision() const {
  return widget->precision;
}

int
OrientationDockBlock::getFieldWidth() const {
  return widget->fieldWidth;
}

double
OrientationDockBlock::getScale() const {
  return widget->scale;
}

bool
OrientationDockBlock::unitVisible() const {
  return widget->unitEnabled;
}

const QString&
OrientationDockBlock::getUnit() const {
  return widget->unit;
}

void
OrientationDockBlock::setFont( const QFont& font ) {
  widget->setFontOfLabel( font );
}

void
OrientationDockBlock::setPrecision( const int precision ) {
  widget->precision = precision;
}

void
OrientationDockBlock::setFieldWidth( const int fieldWidth ) {
  widget->fieldWidth = fieldWidth;
}

void
OrientationDockBlock::setScale( const double scale ) {
  widget->scale = scale;
}

void
OrientationDockBlock::setUnitVisible( const bool enabled ) {
  widget->unitEnabled = enabled;
}

void
OrientationDockBlock::setUnit( const QString& unit ) {
  widget->unit = unit;
}

void
OrientationDockBlock::setName( const QString& name ) {
  BlockBase::setName( name );

  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "Orientation: " ) + name );
}

void
OrientationDockBlock::setOrientation( const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::ValueDock ) ) {
    const auto taitBryanDegrees = radiansToDegrees( quaternionToTaitBryan( orientation ) );
    widget->setValues( getYaw( taitBryanDegrees ), getPitch( taitBryanDegrees ), getRoll( taitBryanDegrees ) );
  }
}

void
OrientationDockBlock::setPose( const Eigen::Vector3d&, const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::ValueDock ) ) {
    setOrientation( orientation );
  }
}

std::unique_ptr< BlockBase >
OrientationDockBlockFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< OrientationDockBlock >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( ValueDockBlockBase::firstThreeValuesDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    ValueDockBlockBase::firstThreeValuesDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, ValueDockBlockBase::firstThreeValuesDock );
  }

  obj->addInputPort( QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  return obj;
}
