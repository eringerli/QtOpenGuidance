// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "helpers/eigenHelper.h"

class MyMainWindow;
class ThreeValuesDock;

#include "ValueDockBlockBase.h"
#include "block/BlockBase.h"

class PositionDockBlock : public ValueDockBlockBase {
  Q_OBJECT

public:
  explicit PositionDockBlock( const QString& uniqueName, MyMainWindow* mainWindow );

  ~PositionDockBlock();

  virtual const QFont&   getFont() override;
  virtual int            getPrecision() override;
  virtual int            getFieldWidth() override;
  virtual double         getScale() override;
  virtual bool           unitVisible() override;
  virtual const QString& getUnit() override;

  virtual void setFont( const QFont& font ) override;
  virtual void setPrecision( const int precision ) override;
  virtual void setFieldWidth( const int fieldWidth ) override;
  virtual void setScale( const double scale ) override;
  virtual void setUnitVisible( const bool enabled ) override;
  virtual void setUnit( const QString& unit ) override;

public Q_SLOTS:
  void setName( const QString& name ) override;

  void setPose( POSE_SIGNATURE_SLOT );

  void setWGS84Position( const Eigen::Vector3d& position );

public:
  ThreeValuesDock* widget = nullptr;

  bool wgs84 = false;
};

class PositionDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  PositionDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "PositionDockBlock" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Display Docks" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Position Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
