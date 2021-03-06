// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "OrientationDockBlock.h"

#include <QAction>
#include <QMenu>
#include <QBrush>

#include "gui/MyMainWindow.h"
#include "gui/dock/ThreeValuesDock.h"

#include "qneblock.h"
#include "qneport.h"

OrientationDockBlock::OrientationDockBlock( const QString& uniqueName, MyMainWindow* mainWindow )
  : ValueDockBlockBase( uniqueName ) {
  widget = new ThreeValuesDock( mainWindow );

  widget->setDescriptions( QStringLiteral( "Y" ), QStringLiteral( "P" ), QStringLiteral( "R" ) );
}

OrientationDockBlock::~OrientationDockBlock() {
  widget->deleteLater();

  if( ValueDockBlockBase::firstThreeValuesDock == dock ) {
    ValueDockBlockBase::firstThreeValuesDock = nullptr;
  }
}

const QFont& OrientationDockBlock::getFont() {
  return widget->fontOfLabel();
}

int OrientationDockBlock::getPrecision() {
  return widget->precision;
}

int OrientationDockBlock::getFieldWidth() {
  return widget->fieldWidth;
}

double OrientationDockBlock::getScale() {
  return widget->scale;
}

bool OrientationDockBlock::unitVisible() {
  return widget->unitEnabled;
}

const QString& OrientationDockBlock::getUnit() {
  return widget->unit;
}

void OrientationDockBlock::setFont( const QFont& font ) {
  widget->setFontOfLabel( font );
}

void OrientationDockBlock::setPrecision( const int precision ) {
  widget->precision = precision;
}

void OrientationDockBlock::setFieldWidth( const int fieldWidth ) {
  widget->fieldWidth = fieldWidth;
}

void OrientationDockBlock::setScale( const double scale ) {
  widget->scale = scale;
}

void OrientationDockBlock::setUnitVisible( const bool enabled ) {
  widget->unitEnabled = enabled;
}

void OrientationDockBlock::setUnit( const QString& unit ) {
  widget->unit = unit;
}

void OrientationDockBlock::setName( const QString& name ) {
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "Orientation: " ) + name );
}

void OrientationDockBlock::setOrientation( const Eigen::Quaterniond& orientation ) {
  const auto taitBryanDegrees = radiansToDegrees( quaternionToTaitBryan( orientation ) );
  widget->setValues( getYaw( taitBryanDegrees ), getPitch( taitBryanDegrees ), getRoll( taitBryanDegrees ) );
}

void OrientationDockBlock::setPose( const Eigen::Vector3d&, const Eigen::Quaterniond& orientation, const PoseOption::Options& ) {
  setOrientation( orientation );
}

QNEBlock* OrientationDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* object = new OrientationDockBlock( getNameOfFactory() + QString::number( id ),
      mainWindow );
  auto* b = createBaseBlock( scene, object, id );

  object->dock->setTitle( getNameOfFactory() );
  object->dock->setWidget( object->widget );

  menu->addAction( object->dock->toggleAction() );

  if( ValueDockBlockBase::firstThreeValuesDock == nullptr ) {
    mainWindow->addDockWidget( object->dock, location );
    ValueDockBlockBase::firstThreeValuesDock = object->dock;
  } else {
    mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnBottom, ValueDockBlockBase::firstThreeValuesDock );
  }

  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( const Eigen::Quaterniond& ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

  b->setBrush( dockColor );

  return b;
}
