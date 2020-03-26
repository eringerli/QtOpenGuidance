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

#pragma once

#include <QObject>
#include <QSizePolicy>
#include <QMenu>

#include "BlockBase.h"

#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>

class ValueDockBlockBase : public BlockBase {
    Q_OBJECT

  public:
    explicit ValueDockBlockBase( const QString& uniqueName )
      : BlockBase() {
      dock = new KDDockWidgets::DockWidget( uniqueName );
    }

    virtual ~ValueDockBlockBase() {
      dock->deleteLater();
    }

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;

      valuesObject[QStringLiteral( "Font" )] = QJsonValue::fromVariant( QVariant( getFont() ) );
      valuesObject[QStringLiteral( "Precision" )] = getPrecision();
      valuesObject[QStringLiteral( "Scale" )] = getScale();
      valuesObject[QStringLiteral( "FieldWitdh" )] = getFieldWidth();
      valuesObject[QStringLiteral( "Unit" )] = getUnit();
      valuesObject[QStringLiteral( "UnitVisible" )] = unitVisible();


      json[QStringLiteral( "values" )] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        setFont( valuesObject[QStringLiteral( "Font" )].toVariant().value<QFont>() );
        setPrecision( valuesObject[QStringLiteral( "Precision" )].toInt( 0 ) );
        setScale( valuesObject[QStringLiteral( "Scale" )].toDouble( 1 ) );
        setFieldWidth( valuesObject[QStringLiteral( "FieldWitdh" )].toInt( 0 ) );
        setUnit( valuesObject[QStringLiteral( "Unit" )].toString( QString() ) );
        setUnitVisible( valuesObject[QStringLiteral( "UnitVisible" )].toBool( false ) );
      }
    }

  public:
    virtual const QFont& getFont() = 0;
    virtual int getPrecision() = 0;
    virtual int getFieldWidth() = 0;
    virtual double getScale() = 0;
    virtual bool unitVisible() = 0;
    virtual const QString& getUnit() = 0;

    virtual void setFont( const QFont& ) = 0;
    virtual void setPrecision( int ) = 0;
    virtual void setFieldWidth( int ) = 0;
    virtual void setScale( double ) = 0;
    virtual void setUnitVisible( bool ) = 0;
    virtual void setUnit( const QString& ) = 0;

  public slots:

  public:
    KDDockWidgets::DockWidget* dock = nullptr;

    static KDDockWidgets::DockWidget* firstValueDock;
    static KDDockWidgets::DockWidget* firstThreeValuesDock;
};

