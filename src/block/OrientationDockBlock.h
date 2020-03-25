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

#include "../cgalKernel.h"

#include "../gui/MyMainWindow.h"
#include "../gui/ThreeValuesDock.h"

#include "BlockBase.h"
#include "ValueDockBlockBase.h"

#include "../kinematic/PoseOptions.h"

class OrientationDockBlock : public ValueDockBlockBase {
    Q_OBJECT

  public:
    explicit OrientationDockBlock( const QString& uniqueName,
                                   MyMainWindow* mainWindow )
      : ValueDockBlockBase( uniqueName ) {
      widget = new ThreeValuesDock( mainWindow );

      widget->setDescriptions( QStringLiteral( "R" ), QStringLiteral( "P" ), QStringLiteral( "H" ) );
    }

    ~OrientationDockBlock() {
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
      dock->toggleAction()->setText( QStringLiteral( "Orientation: " ) + name );
    }

    void setOrientation( QQuaternion orientation ) {
      QVector3D eulerAngles = orientation.toEulerAngles();

      widget->setValues( eulerAngles.y(), eulerAngles.x(), eulerAngles.z() );
    }

    void setPose( const Point_3&, const QQuaternion orientation, const PoseOption::Options ) {
      setOrientation( orientation );
    }

  public:
    ThreeValuesDock* widget = nullptr;
};

class OrientationDockBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    OrientationDockBlockFactory( MyMainWindow* mainWindow,
                                 KDDockWidgets::Location location,
                                 QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "OrientationDockBlock" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
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

      b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( QQuaternion ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );

      b->setBrush( QColor( QStringLiteral( "lightsalmon" ) ) );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
};

