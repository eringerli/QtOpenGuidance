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

#include "helpers/eigenHelper.h"

#include <QObject>

#include "block/BlockBase.h"

class MyMainWindow;
class ImplementSection;
class ImplementBlockModel;
class SectionControlToolbar;

#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>

class Implement : public BlockBase {
  Q_OBJECT

public:
  explicit Implement( const QString& uniqueName, MyMainWindow* mainWindow, KDDockWidgets::DockWidget** firstDock );

  ~Implement() override;

  void emitConfigSignals() override;

  QJsonObject toJSON() override;
  void        fromJSON( QJsonObject& ) override;

  void emitImplementChanged();
  void emitSectionsChanged();

  bool isSectionOn( const size_t section );

Q_SIGNALS:
  void triggerLocalPose( POSE_SIGNATURE_SIGNAL );
  void leftEdgeChanged( const Eigen::Vector3d& );
  void rightEdgeChanged( const Eigen::Vector3d& );
  void implementChanged( const QPointer< Implement > );
  void sectionsChanged();

public Q_SLOTS:
  void setName( const QString& name ) override;

public:
  KDDockWidgets::DockWidget* dock   = nullptr;
  SectionControlToolbar*     widget = nullptr;

  std::vector< ImplementSection* > sections;

private:
  KDDockWidgets::DockWidget** firstDock = nullptr;
};

class ImplementFactory : public BlockFactory {
  Q_OBJECT

public:
  ImplementFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu, ImplementBlockModel* model )
      : BlockFactory( thread ), mainWindow( mainWindow ), location( location ), menu( menu ), model( model ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Implement" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Section Control" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  MyMainWindow*              mainWindow = nullptr;
  KDDockWidgets::Location    location;
  QMenu*                     menu      = nullptr;
  ImplementBlockModel*       model     = nullptr;
  KDDockWidgets::DockWidget* firstDock = nullptr;
};
