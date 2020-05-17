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

#include <QQuaternion>
#include <QVector3D>

#include <QDebug>

#include "BlockBase.h"
#include "../gui/ActionToolbar.h"

#include "../gui/MyMainWindow.h"
#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"

class ActionDockBlock : public BlockBase {
    Q_OBJECT

  public:
    explicit ActionDockBlock( const QString& uniqueName,
                              MyMainWindow* mainWindow )
      : BlockBase() {
      widget = new ActionToolbar( mainWindow );
      dock = new KDDockWidgets::DockWidget( uniqueName );

      QObject::connect( widget, &ActionToolbar::action, this, &ActionDockBlock::action );
    }

    ~ActionDockBlock();

  public slots:
    void setName( const QString& name ) override {
      dock->setTitle( name );
      dock->toggleAction()->setText( QStringLiteral( "Action: " ) + name );
    }

    void setCheckable( bool checkable ) {
      widget->setCheckable( checkable );
    }

    void setTheme( const QString theme ) {
      widget->setTheme( theme );
    }

  signals:
    void action( bool );

  public:
    ActionToolbar* widget = nullptr;
    KDDockWidgets::DockWidget* dock = nullptr;
};

class ActionDockBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    ActionDockBlockFactory( MyMainWindow* mainWindow,
                            KDDockWidgets::Location location,
                            QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Action Dock Block" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* object = new ActionDockBlock( getNameOfFactory() + QString::number( id ),
                                          mainWindow );
      auto* b = createBaseBlock( scene, object, id );

      object->dock->setTitle( getNameOfFactory() );
      object->dock->setWidget( object->widget );

      menu->addAction( object->dock->toggleAction() );

      if( firstActionDock == nullptr ) {
        mainWindow->addDockWidget( object->dock, location );
        firstActionDock = object->dock;
      } else {
        mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnBottom, firstActionDock );
      }

      b->addOutputPort( QStringLiteral( "Action with State" ), QLatin1String( SIGNAL( action( bool ) ) ) );

      b->setBrush( inputDockColor );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;

  public:
    static KDDockWidgets::DockWidget* firstActionDock;
};
