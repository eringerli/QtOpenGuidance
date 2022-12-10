// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MyMainWindow;
class ThreeValuesDock;

#include "ValueDockBlockBase.h"
#include "block/BlockBase.h"

#include "helpers/anglesHelper.h"
#include "helpers/eigenHelper.h"

class OrientationDockBlock : public ValueDockBlockBase {
  Q_OBJECT

public:
  explicit OrientationDockBlock( const QString& uniqueName, MyMainWindow* mainWindow );

  ~OrientationDockBlock();

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

  void setOrientation( const Eigen::Quaterniond& orientation );

  void setPose( POSE_SIGNATURE_SLOT );

public:
  ThreeValuesDock* widget = nullptr;
};

class OrientationDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  OrientationDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "OrientationDockBlock" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Display Docks" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Orientation Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
