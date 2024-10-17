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
  explicit Implement( const QString&                         uniqueName,
                      MyMainWindow*                          mainWindow,
                      KDDockWidgets::QtWidgets::DockWidget** firstDock,
                      const BlockBaseId                      idHint,
                      const bool                             systemBlock,
                      const QString                          type,
                      const BlockBase::TypeColor             typeColor );

  ~Implement() override;

  void emitConfigSignals() override;

  void toJSON( QJsonObject& json ) const override;
  void fromJSON( const QJsonObject& json ) override;

  void emitImplementChanged();
  void emitSectionsChanged();

  bool isSectionOn( const size_t section );

Q_SIGNALS:
  void triggerLocalPose( POSE_SIGNATURE_SIGNAL );
  void leftEdgeChanged( VECTOR_SIGNATURE_SIGNAL );
  void rightEdgeChanged( VECTOR_SIGNATURE_SIGNAL );
  void implementChanged( const QPointer< Implement > );
  void sectionsChanged();

public Q_SLOTS:
  void setName( const QString& name ) override;

public:
  KDDockWidgets::QtWidgets::DockWidget* dock   = nullptr;
  ImplementToolbar*                     widget = nullptr;

  std::vector< ImplementSection* > sections;

private:
  KDDockWidgets::QtWidgets::DockWidget** firstDock = nullptr;
};

class ImplementFactory : public BlockFactory {
  Q_OBJECT

public:
  ImplementFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu, ImplementBlockModel* model )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ), model( model ) {
    typeColor = BlockBase::TypeColor::Arithmetic;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Implement" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Section Control" ); }

  virtual std::unique_ptr< BlockBase > createBlock( BlockBaseId idHint = 0 ) override;

private:
  MyMainWindow*                         mainWindow = nullptr;
  KDDockWidgets::Location               location;
  QMenu*                                menu      = nullptr;
  ImplementBlockModel*                  model     = nullptr;
  KDDockWidgets::QtWidgets::DockWidget* firstDock = nullptr;
};
