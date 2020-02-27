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

#include "../gui/ImplementBlockModel.h"

#include "../gui/SectionControlToolbar.h"

#pragma once

#include "../cgalKernel.h"

#include <QObject>

#include <QQuaternion>
#include <QVector3D>
#include <QMenu>

#include "BlockBase.h"

#include "ImplementSection.h"

#include "../kinematic/PoseOptions.h"

#include "../gui/MyMainWindow.h"

#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>

class Implement : public BlockBase {
    Q_OBJECT

  public:
    explicit Implement( QString uniqueName,
                        MyMainWindow* mainWindow )
      : BlockBase() {
      widget = new SectionControlToolbar( this, mainWindow );
      dock = new KDDockWidgets::DockWidget( uniqueName );

//      QObject::connect( dock, &QDockWidget::dockLocationChanged, widget, &SectionControlToolbar::setDockLocation );

      // add section 0: the section to control them all
      sections.push_back( new ImplementSection( 0, 0, 0 ) );
    }

    ~Implement() override {
      widget->deleteLater();
      dock->deleteLater();
    }

    void emitConfigSignals() override {
      double width = 0;

      for( const auto& section : qAsConst( sections ) ) {
        width +=  section->widthOfSection - section->overlapLeft - section->overlapRight;
      }

      emit leftEdgeChanged( QVector3D( 0, float( -width / 2 ), 0 ) );
      emit rightEdgeChanged( QVector3D( 0, float( width / 2 ), 0 ) );
      auto dummyPoint = Point_3();
      auto dummyQuaternion = QQuaternion();
      auto dummyFlags = PoseOption::CalculateLocalOffsets |
                        PoseOption::CalculateWithoutOrientation |
                        PoseOption::CalculateFromPivotPoint;
      emit triggerLocalPose( dummyPoint, dummyQuaternion, dummyFlags );
      emit implementChanged( this );
    }

    void toJSON( QJsonObject& json ) override {
      if( sections.size() > 1 ) {
        QJsonArray array;

        for( size_t i = 1; i < sections.size(); ++i ) {
          QJsonObject sectionObject;
          sectionObject[QStringLiteral( "overlapLeft" )] = sections[i]->overlapLeft;
          sectionObject[QStringLiteral( "widthOfSection" )] = sections[i]->widthOfSection;
          sectionObject[QStringLiteral( "overlapRight" )] = sections[i]->overlapRight;
          array.append( sectionObject );
        }

        QJsonObject valuesObject;
        valuesObject[QStringLiteral( "Sections" )] = array;
        json[QStringLiteral( "values" )] = valuesObject;
      }
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        if( valuesObject[QStringLiteral( "Sections" )].isArray() ) {
          QJsonArray sectionArray = valuesObject[QStringLiteral( "Sections" )].toArray();

          for( const auto& sectionIndex : qAsConst( sectionArray ) ) {
            QJsonObject sectionObject = sectionIndex.toObject();
            sections.push_back(
                    new ImplementSection( sectionObject[QStringLiteral( "overlapLeft" )].toDouble( 0 ),
                                          sectionObject[QStringLiteral( "widthOfSection" )].toDouble( 0 ),
                                          sectionObject[QStringLiteral( "overlapRight" )].toDouble( 0 ) ) );
          }
        }
      }
    }

    void emitImplementChanged() {
      emit implementChanged( QPointer<Implement>( this ) );

    }

    void emitSectionsChanged() {
      emit sectionsChanged();
    }

  signals:
    void triggerLocalPose( const Point_3&, const QQuaternion, const PoseOption::Options );
    void leftEdgeChanged( QVector3D );
    void rightEdgeChanged( QVector3D );
    void implementChanged( const QPointer<Implement> );
    void sectionsChanged();

  public slots:
    void setName( const QString& name ) override {
      dock->setTitle( name );
      dock->toggleAction()->setText( QStringLiteral( "SC: " ) + name );
    }

  public:
    KDDockWidgets::DockWidget* dock = nullptr;
    SectionControlToolbar* widget = nullptr;

    std::vector<ImplementSection*> sections;
};

class ImplementFactory : public BlockFactory {
    Q_OBJECT

  public:
    ImplementFactory( MyMainWindow* mainWindow,
                      KDDockWidgets::Location location,
                      QMenu* menu,
                      ImplementBlockModel* model )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ),
        model( model ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Implement" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      if( id != 0 && !isIdUnique( scene, id ) ) {
        id = QNEBlock::getNextUserId();
      }

      auto* object = new Implement( getNameOfFactory() + QString::number( id ),
                                    mainWindow );
      auto* b = createBaseBlock( scene, object, id );

      object->dock->setTitle( getNameOfFactory() );
      object->dock->setWidget( object->widget );

      menu->addAction( object->dock->toggleAction() );

      if( firstDock == nullptr ) {
        mainWindow->addDockWidget( object->dock, location );
        firstDock = object->dock;
      } else {
        mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnBottom, firstDock );
      }

      b->addOutputPort( QStringLiteral( "Trigger Calculation of Local Pose" ), QLatin1String( SIGNAL( triggerLocalPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Implement Data" ), QLatin1String( SIGNAL( implementChanged( const QPointer<Implement> ) ) ) );
      b->addOutputPort( QStringLiteral( "Section Control Data" ), QLatin1String( SIGNAL( sectionsChanged() ) ) );
      b->addOutputPort( QStringLiteral( "Position Left Edge" ), QLatin1String( SIGNAL( leftEdgeChanged( QVector3D ) ) ) );
      b->addOutputPort( QStringLiteral( "Position Right Edge" ), QLatin1String( SIGNAL( rightEdgeChanged( QVector3D ) ) ) );

      model->resetModel();

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
    ImplementBlockModel* model = nullptr;
    KDDockWidgets::DockWidget* firstDock = nullptr;
};

