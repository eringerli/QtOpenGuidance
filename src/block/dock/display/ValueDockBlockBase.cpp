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

#include "ValueDockBlockBase.h"

#include "gui/MyMainWindow.h"

#include <QBrush>
#include <QJsonObject>

KDDockWidgets::DockWidget* ValueDockBlockBase::firstValueDock       = nullptr;
KDDockWidgets::DockWidget* ValueDockBlockBase::firstThreeValuesDock = nullptr;

void
ValueDockBlockBase::toJSON( QJsonObject& json ) {
  QJsonObject valuesObject;

  valuesObject[QStringLiteral( "Font" )]        = QJsonValue::fromVariant( QVariant( getFont() ) );
  valuesObject[QStringLiteral( "Precision" )]   = getPrecision();
  valuesObject[QStringLiteral( "Scale" )]       = getScale();
  valuesObject[QStringLiteral( "FieldWitdh" )]  = getFieldWidth();
  valuesObject[QStringLiteral( "Unit" )]        = getUnit();
  valuesObject[QStringLiteral( "UnitVisible" )] = unitVisible();

  json[QStringLiteral( "values" )] = valuesObject;
}

void
ValueDockBlockBase::fromJSON( QJsonObject& json ) {
  if( json[QStringLiteral( "values" )].isObject() ) {
    QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

    setFont( valuesObject[QStringLiteral( "Font" )].toVariant().value< QFont >() );
    setPrecision( valuesObject[QStringLiteral( "Precision" )].toInt( 0 ) );
    setScale( valuesObject[QStringLiteral( "Scale" )].toDouble( 1 ) );
    setFieldWidth( valuesObject[QStringLiteral( "FieldWitdh" )].toInt( 0 ) );
    setUnit( valuesObject[QStringLiteral( "Unit" )].toString( QString() ) );
    setUnitVisible( valuesObject[QStringLiteral( "UnitVisible" )].toBool( false ) );
  }
}
