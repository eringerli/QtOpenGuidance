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

#ifndef POSITIONDOCKBLOCK_H
#define POSITIONDOCKBLOCK_H

#include <QObject>
#include <QDockWidget>
#include <QSizePolicy>
#include <QMenu>

#include "../gui/MainWindow.h"
#include "../gui/ThreeValuesDock.h"

#include "BlockBase.h"
#include "ValueDockBlockBase.h"

#include "../kinematic/PoseOptions.h"

class PositionDockBlock : public ValueDockBlockBase {
    Q_OBJECT

  public:
    explicit PositionDockBlock( MainWindow* mainWindow )
      : ValueDockBlockBase( mainWindow ) {
      widget = new ThreeValuesDock( mainWindow );

      widget->setDescriptions( QStringLiteral( "X" ), QStringLiteral( "Y" ), QStringLiteral( "Z" ) );
    }

    ~PositionDockBlock() {
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
      dock->setWindowTitle( name );
      action->setText( QStringLiteral( "Position: " ) + name );
      widget->setName( name );
    }

    void setPose( const Point_3& point, const QQuaternion, const PoseOption::Options ) {
      if( wgs84 ) {
        widget->setDescriptions( QStringLiteral( "X" ), QStringLiteral( "Y" ), QStringLiteral( "Z" ) );
      }

      widget->setValues( point.x(), point.y(), point.z() );
    }

    void setWGS84Position( double lat, double lon, double height ) {
      if( !wgs84 ) {
        widget->setDescriptions( QStringLiteral( "Lat" ), QStringLiteral( "Lon" ), QStringLiteral( "H" ) );
      }

      widget->setValues( lat, lon, height );
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

    bool wgs84 = false;
};

class PositionDockBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    PositionDockBlockFactory( MainWindow* mainWindow,
                              Qt::DockWidgetArea area,
                              Qt::DockWidgetAreas allowedAreas,
                              QDockWidget::DockWidgetFeatures features,
                              QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        area( area ),
        allowedAreas( allowedAreas ),
        features( features ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "PositionDockBlock" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new PositionDockBlock( mainWindow );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      PositionDockBlock* object = qobject_cast<PositionDockBlock*>( obj );

      object->dock->setWidget( object->widget );
      object->dock->setFeatures( features );
      object->dock->setAllowedAreas( allowedAreas );
      object->dock->setObjectName( getNameOfFactory() + QString::number( b->id ) );

      object->action = object->dock->toggleViewAction();
      menu->addAction( object->action );

      mainWindow->addDockWidget( area, object->dock );

      b->addInputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SLOT( setWGS84Position( const double, const double, const double ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );

      return b;
    }

  private:
    MainWindow* mainWindow = nullptr;
    Qt::DockWidgetArea area;
    Qt::DockWidgetAreas allowedAreas;
    QDockWidget::DockWidgetFeatures features;
    QMenu* menu = nullptr;
};

#endif // POSITIONDOCKBLOCK_H
