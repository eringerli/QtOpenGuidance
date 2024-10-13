// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueDockBlockBase.h"

#include "block/BlockBase.h"
#include "gui/MyMainWindow.h"

#include <QJsonObject>

KDDockWidgets::QtWidgets::DockWidget* ValueDockBlockBase::firstValueDock       = nullptr;
KDDockWidgets::QtWidgets::DockWidget* ValueDockBlockBase::firstThreeValuesDock = nullptr;

void
ValueDockBlockBase::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "Font" )]        = QJsonValue::fromVariant( QVariant( getFont() ) );
  valuesObject[QStringLiteral( "Precision" )]   = getPrecision();
  valuesObject[QStringLiteral( "Scale" )]       = getScale();
  valuesObject[QStringLiteral( "FieldWitdh" )]  = getFieldWidth();
  valuesObject[QStringLiteral( "Unit" )]        = getUnit();
  valuesObject[QStringLiteral( "UnitVisible" )] = unitVisible();
}

void
ValueDockBlockBase::fromJSON( const QJsonObject& valuesObject ) {
  setFont( valuesObject[QStringLiteral( "Font" )].toVariant().value< QFont >() );
  setPrecision( valuesObject[QStringLiteral( "Precision" )].toInt( 0 ) );
  setScale( valuesObject[QStringLiteral( "Scale" )].toDouble( 1 ) );
  setFieldWidth( valuesObject[QStringLiteral( "FieldWitdh" )].toInt( 0 ) );
  setUnit( valuesObject[QStringLiteral( "Unit" )].toString( QString() ) );
  setUnitVisible( valuesObject[QStringLiteral( "UnitVisible" )].toBool( false ) );
}
