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

#ifndef ORIENTATIONDOCKBLOCK_H
#define ORIENTATIONDOCKBLOCK_H

#include <QObject>
#include <QDockWidget>
#include <QSizePolicy>
#include <QMenu>

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
    }

    virtual const QFont& getFont() {
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
    virtual bool captionEnabled() {
      return widget->captionEnabled();
    }

    virtual void setFont( const QFont& font ) {
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
    virtual void setCaptionEnabled( bool enabled ) override {
      widget->setCaptionEnabled( enabled );
    }

  public slots:
    void setName( const QString& name ) override {
      dock->setTitle( name );
      dock->toggleAction()->setText( QStringLiteral( "Orientation: " ) + name );
      widget->setName( name );
    }

    void setOrientation( QQuaternion orientation ) {
      QVector3D eulerAngles = orientation.toEulerAngles();

      widget->setValues( eulerAngles.y(), eulerAngles.x(), eulerAngles.z() );
    }

    void setPose( const Point_3&, const QQuaternion orientation, const PoseOption::Options ) {
      setOrientation( orientation );
    }

  public:

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;

      valuesObject[QStringLiteral( "Font" )] = QJsonValue::fromVariant( QVariant( widget->fontOfLabel() ) );
      valuesObject[QStringLiteral( "Precision" )] = widget->precision;
      valuesObject[QStringLiteral( "Scale" )] = widget->scale;
      valuesObject[QStringLiteral( "FieldWitdh" )] = widget->fieldWidth;

      json[QStringLiteral( "values" )] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        widget->setFontOfLabel( valuesObject[QStringLiteral( "Font" )].toVariant().value<QFont>() );
        widget->precision = valuesObject[QStringLiteral( "Precision" )].toInt();
        widget->scale = valuesObject[QStringLiteral( "Scale" )].toDouble();
        widget->fieldWidth = valuesObject[QStringLiteral( "FieldWitdh" )].toInt();
      }
    }

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

    virtual QNEBlock* createBlock( QGraphicsScene* scene ) override {
      int id = QNEBlock::getNextUserId();
      auto* object = new OrientationDockBlock( getNameOfFactory() + QString::number( id ),
          mainWindow );
      auto* b = createBaseBlock( scene, object, false, id );

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

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
};

#endif // ORIENTATIONDOCKBLOCK_H
