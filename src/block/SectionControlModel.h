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

#ifndef SECTIONCONTROLMODEL_H
#define SECTIONCONTROLMODEL_H

#include <QObject>
#include <QDockWidget>
#include <QMenu>

#include "BlockBase.h"

#include "../block/Implement.h"

#include "../gui/MainWindow.h"
#include "../gui/SectionControlToolbar.h"

class SectionControlModel : public BlockBase {
    Q_OBJECT

  public:
    explicit SectionControlModel( MainWindow* mainWindow )
      : BlockBase() {
      widget = new SectionControlToolbar( mainWindow );
      dock = new QDockWidget( mainWindow );
    }

    ~SectionControlModel() {
      widget->deleteLater();
      dock->deleteLater();
    }


  public slots:
    void setName( QString name ) {
      dock->setWindowTitle( name );
      action->setText( QStringLiteral( "SC: " ) + name );
    }

    void setSections( const QVector<QSharedPointer<ImplementSection>>& sections ) {
      this->sections = sections;
      widget->setNumberOfSections( sections.count() );
    }

  signals:
    void sectionStateChanged( int, int );

  public:
    QDockWidget* dock = nullptr;
    QAction* action = nullptr;
    SectionControlToolbar* widget = nullptr;

    QVector<QSharedPointer<ImplementSection>> sections;
};

class SectionControlModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    SectionControlModelFactory( MainWindow* mainWindow,
                                Qt::DockWidgetArea area,
                                Qt::DockWidgetAreas allowedAreas,
                                QDockWidget::DockWidgetFeatures features,
                                QMenu* menu,
                                QDockWidget* dockToSplit )
      : BlockFactory(),
        mainWindow( mainWindow ),
        area( area ),
        allowedAreas( allowedAreas ),
        features( features ),
        menu( menu ),
        dockToSplit( dockToSplit ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "SectionControl Model" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new SectionControlModel( mainWindow );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      SectionControlModel* object = qobject_cast<SectionControlModel*>( obj );

      object->dock->setWidget( object->widget );
      object->dock->setFeatures( features );
      object->dock->setAllowedAreas( allowedAreas );
      object->dock->setObjectName( getNameOfFactory() + QString::number( b->id ) );

      object->action = object->dock->toggleViewAction();
      menu->addAction( object->action );

      mainWindow->addDockWidget( area, object->dock );
      mainWindow->splitDockWidget( dockToSplit, object->dock, Qt::Vertical );

      b->addInputPort( "Section Controll Data", SLOT( setSections( QVector<QSharedPointer<ImplementSection>> ) ) );

      return b;
    }

  private:
    MainWindow* mainWindow = nullptr;
    Qt::DockWidgetArea area;
    Qt::DockWidgetAreas allowedAreas;
    QDockWidget::DockWidgetFeatures features;
    QMenu* menu = nullptr;
    QDockWidget* dockToSplit = nullptr;
};

#endif // SECTIONCONTROLMODEL_H
