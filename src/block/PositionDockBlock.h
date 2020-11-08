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
#include <QDockWidget>
#include <QSizePolicy>
#include <QMenu>

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

#include "../gui/MyMainWindow.h"
#include "../gui/ThreeValuesDock.h"

#include "BlockBase.h"
#include "ValueDockBlockBase.h"

class PositionDockBlock : public ValueDockBlockBase {
    Q_OBJECT

  public:
    explicit PositionDockBlock( const QString& uniqueName,
                                MyMainWindow* mainWindow )
      : ValueDockBlockBase( uniqueName ) {
      widget = new ThreeValuesDock( mainWindow );

      widget->setDescriptions( QStringLiteral( "X" ), QStringLiteral( "Y" ), QStringLiteral( "Z" ) );
    }

    ~PositionDockBlock() {
      widget->deleteLater();

      if( ValueDockBlockBase::firstThreeValuesDock == dock ) {
        ValueDockBlockBase::firstThreeValuesDock = nullptr;
      }
    }

    virtual const QFont& getFont() override {
      return widget->fontOfLabel();
    }
    virtual int getPrecision() override {
      return widget->precision;
    }
    virtual int getFieldWidth() override {
      return widget->fieldWidth;
    }
    virtual double getScale() override {
      return widget->scale;
    }
    virtual bool unitVisible() override {
      return widget->unitEnabled;
    }
    virtual const QString& getUnit() override {
      return widget->unit;
    }

    virtual void setFont( const QFont& font ) override {
      widget->setFontOfLabel( font );
    }
    virtual void setPrecision( int precision ) override {
      widget->precision = precision;
    }
    virtual void setFieldWidth( int fieldWidth ) override {
      widget->fieldWidth = fieldWidth;
    }
    virtual void setScale( double scale ) override {
      widget->scale = scale;
    }
    virtual void setUnitVisible( bool enabled ) override {
      widget->unitEnabled = enabled;
    }
    virtual void setUnit( const QString& unit ) override {
      widget->unit = unit;
    }

  public slots:
    void setName( const QString& name ) override {
      dock->setTitle( name );
      dock->toggleAction()->setText( QStringLiteral( "Position: " ) + name );
    }

    void setPose( const Point_3 point, const Eigen::Quaterniond, const PoseOption::Options ) {
      if( wgs84 ) {
        widget->setDescriptions( QStringLiteral( "X" ), QStringLiteral( "Y" ), QStringLiteral( "Z" ) );
      }

      widget->setValues( point.x(), point.y(), point.z() );
    }

    void setWGS84Position( const Eigen::Vector3d position ) {
      if( !wgs84 ) {
        widget->setDescriptions( QStringLiteral( "Lon" ), QStringLiteral( "Lat" ), QStringLiteral( "H" ) );
      }

      widget->setValues( position.x(), position.y(), position.z() );
    }

  public:
    ThreeValuesDock* widget = nullptr;

    bool wgs84 = false;
};

class PositionDockBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    PositionDockBlockFactory( MyMainWindow* mainWindow,
                              KDDockWidgets::Location location,
                              QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "PositionDockBlock" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Display Docks" );
    }

    QString getPrettyNameOfFactory() override {
      return QStringLiteral( "Position Dock" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      if( id != 0 && !isIdUnique( scene, id ) ) {
        id = QNEBlock::getNextUserId();
      }

      auto* object = new PositionDockBlock( getNameOfFactory() + QString::number( id ),
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

      b->addInputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SLOT( setWGS84Position( const Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      b->setBrush( dockColor );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
};
