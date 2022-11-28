// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "helpers/eigenHelper.h"

#include <QObject>

#include "block/BlockBase.h"

class MyMainWindow;
class ImplementSection;
class ImplementBlockModel;
class ImplementToolbar;

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
  ImplementToolbar*          widget = nullptr;

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
