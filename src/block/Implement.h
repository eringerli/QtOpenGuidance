// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef IMPLEMENT_H
#define IMPLEMENT_H

#include "../cgalKernel.h"

#include <QObject>

#include <QQuaternion>
#include <QVector3D>
#include <QDockWidget>
#include <QMenu>

#include "BlockBase.h"

#include "ImplementSection.h"

#include "../kinematic/PoseOptions.h"

#include "../gui/MainWindow.h"

class Implement : public BlockBase {
    Q_OBJECT

  public:
    explicit Implement( MainWindow* mainWindow )
      : BlockBase() {
      widget = new SectionControlToolbar( this, mainWindow );
      dock = new QDockWidget( mainWindow );

      QObject::connect( dock, &QDockWidget::dockLocationChanged, widget, &SectionControlToolbar::setDockLocation );

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
      dock->setWindowTitle( name );
      action->setText( QStringLiteral( "SC: " ) + name );
    }

  public:
    QDockWidget* dock = nullptr;
    QAction* action = nullptr;
    SectionControlToolbar* widget = nullptr;

    std::vector<ImplementSection*> sections;
};

class ImplementFactory : public BlockFactory {
    Q_OBJECT

  public:
    ImplementFactory( MainWindow* mainWindow,
                      Qt::DockWidgetArea area,
                      Qt::DockWidgetAreas allowedAreas,
                      QDockWidget::DockWidgetFeatures features,
                      QMenu* menu,
                      QDockWidget* dockToSplit,
                      ImplementBlockModel* model )
      : BlockFactory(),
        mainWindow( mainWindow ),
        area( area ),
        allowedAreas( allowedAreas ),
        features( features ),
        menu( menu ),
        dockToSplit( dockToSplit ),
        model( model ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Implement" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new Implement( mainWindow );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      Implement* object = qobject_cast<Implement*>( obj );

      object->dock->setWidget( object->widget );
      object->dock->setFeatures( features );
      object->dock->setAllowedAreas( allowedAreas );
      object->dock->setObjectName( getNameOfFactory() + QString::number( b->id ) );

      object->action = object->dock->toggleViewAction();
      menu->addAction( object->action );

      mainWindow->addDockWidget( area, object->dock );
      mainWindow->splitDockWidget( dockToSplit, object->dock, Qt::Vertical );

      b->addOutputPort( QStringLiteral( "Trigger Calculation of Local Pose" ), QLatin1String( SIGNAL( triggerLocalPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Implement Data" ), QLatin1String( SIGNAL( implementChanged( const QPointer<Implement> ) ) ) );
      b->addOutputPort( QStringLiteral( "Section Control Data" ), QLatin1String( SIGNAL( sectionsChanged() ) ) );
      b->addOutputPort( QStringLiteral( "Position Left Edge" ), QLatin1String( SIGNAL( leftEdgeChanged( QVector3D ) ) ) );
      b->addOutputPort( QStringLiteral( "Position Right Edge" ), QLatin1String( SIGNAL( rightEdgeChanged( QVector3D ) ) ) );

      model->resetModel();

      return b;
    }

  private:
    MainWindow* mainWindow = nullptr;
    Qt::DockWidgetArea area;
    Qt::DockWidgetAreas allowedAreas;
    QDockWidget::DockWidgetFeatures features;
    QMenu* menu = nullptr;
    QDockWidget* dockToSplit = nullptr;
    ImplementBlockModel* model = nullptr;
};

#endif // IMPLEMENT_H
