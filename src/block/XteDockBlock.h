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

#ifndef XTEDOCKBLOCK_H
#define XTEDOCKBLOCK_H

#include <QObject>
#include <QDockWidget>
#include <QMenu>

#include "../gui/MyMainWindow.h"
#include "../gui/XteDock.h"

#include "BlockBase.h"

#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>

class XteDockBlock : public BlockBase {
    Q_OBJECT

  public:
    explicit XteDockBlock( const QString& uniqueName,
                           MyMainWindow* mainWindow )
      : BlockBase() {
      widget = new XteDock( mainWindow );
      dock = new KDDockWidgets::DockWidget( uniqueName );
    }

    ~XteDockBlock() {
      widget->deleteLater();
      dock->deleteLater();
    }


  public slots:
    void setName( const QString& name ) override {
      dock->setTitle( name );
      dock->toggleAction()->setText( QStringLiteral( "SC: " ) + name );
      widget->setName( name );
    }

    void setXte( float xte ) {
      widget->setXte( xte );
    }

  public:
    KDDockWidgets::DockWidget* dock = nullptr;
    XteDock* widget = nullptr;
};

class XteDockBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    XteDockBlockFactory( MyMainWindow* mainWindow,
                         KDDockWidgets::Location location,
                         QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "XteDockBlock" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene ) override {
      int id = QNEBlock::getNextUserId();
      auto* object = new XteDockBlock( getNameOfFactory() + QString::number( id ),
                                       mainWindow );
      auto* b = createBaseBlock( scene, object, false, id );

      object->dock->setTitle( getNameOfFactory() );
      object->dock->setWidget( object->widget );

      menu->addAction( object->dock->toggleAction() );

      if( firstDock == nullptr ) {
        mainWindow->addDockWidget( object->dock, location );
        firstDock = object->dock;
      } else {
        mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnRight, firstDock );
      }

      b->addInputPort( QStringLiteral( "XTE" ), QLatin1String( SLOT( setXte( float ) ) ) );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
    KDDockWidgets::DockWidget* firstDock = nullptr;
};

#endif // XTEDOCKBLOCK_H
