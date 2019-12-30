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
      sections.append(
              QSharedPointer<ImplementSection>(
                      new ImplementSection( 0, 0, 0 ) ) );
    }

    ~Implement() override {
      widget->deleteLater();
      dock->deleteLater();
    }

    void emitConfigSignals() override {
      double width = 0;

      foreach( const QSharedPointer<ImplementSection> section, sections ) {
        width +=  section->widthOfSection - section->overlapLeft - section->overlapRight;
      }

      emit leftEdgeChanged( QVector3D( 0, float( -width / 2 ), 0 ) );
      emit rightEdgeChanged( QVector3D( 0, float( width / 2 ), 0 ) );
      emit triggerLocalPose( Point_3(), QQuaternion(),
                             PoseOption::CalculateLocalOffsets |
                             PoseOption::CalculateWithoutOrientation |
                             PoseOption::CalculateFromPivotPoint );
      emit implementChanged();
    }

    void toJSON( QJsonObject& json ) override {
      if( sections.size() > 1 ) {
        QJsonArray array;

//      foreach( const QSharedPointer<ImplementSection> section, sections ) {
        for( int i = 1; i < sections.size(); ++i ) {
          QJsonObject sectionObject;
          sectionObject["overlapLeft"] = sections[i]->overlapLeft;
          sectionObject["widthOfSection"] = sections[i]->widthOfSection;
          sectionObject["overlapRight"] = sections[i]->overlapRight;
          array.append( sectionObject );
        }

        QJsonObject valuesObject;
        valuesObject["Sections"] = array;
        json["values"] = valuesObject;
      }
    }

    void fromJSON( QJsonObject& json ) override {
      if( json["values"].isObject() ) {
        QJsonObject valuesObject = json["values"].toObject();

        if( valuesObject["Sections"].isArray() ) {
          QJsonArray sectionArray = valuesObject["Sections"].toArray();

          for( auto sectionIndex : sectionArray ) {
            QJsonObject sectionObject = sectionIndex.toObject();
            sections.append(
                    QSharedPointer<ImplementSection>(
                            new ImplementSection( sectionObject["overlapLeft"].toDouble( 0 ),
                                                  sectionObject["widthOfSection"].toDouble( 0 ),
                                                  sectionObject["overlapRight"].toDouble( 0 ) ) ) );
          }
        }
      }
    }

    void emitImplementChanged() {
      emit implementChanged();
    }

    void emitSectionsChanged() {
      emit sectionsChanged();
    }

  signals:
    void triggerLocalPose( Point_3, QQuaternion, PoseOption::Options );
    void leftEdgeChanged( QVector3D );
    void rightEdgeChanged( QVector3D );
    void implementChanged();
    void sectionsChanged();

  public slots:
    void setName( QString name ) override {
      dock->setWindowTitle( name );
      action->setText( QStringLiteral( "SC: " ) + name );
    }

  public:
    QDockWidget* dock = nullptr;
    QAction* action = nullptr;
    SectionControlToolbar* widget = nullptr;

    QVector<QSharedPointer<ImplementSection>> sections;
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

      b->addOutputPort( "Trigger Calculation of Local Pose", SIGNAL( triggerLocalPose( Point_3, QQuaternion, PoseOption::Options ) ) );
//      b->addOutputPort( "Section Control Data", SIGNAL( implementChanged( QPointer<Implement> ) ) );
      b->addOutputPort( "Position Left Edge", SIGNAL( leftEdgeChanged( QVector3D ) ) );
      b->addOutputPort( "Position Right Edge", SIGNAL( rightEdgeChanged( QVector3D ) ) );

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
