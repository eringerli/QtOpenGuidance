// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "helpers/signatures.h"
#include <QObject>

class MyMainWindow;
class ThreeValuesDock;

#include "ValueDockBlockBase.h"
#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"
#include "helpers/anglesHelper.h"
#include "helpers/eigenHelper.h"

class OrientationDockBlock : public ValueDockBlockBase {
  Q_OBJECT

public:
  explicit OrientationDockBlock(
    MyMainWindow* mainWindow, const QString& uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString type );

  ~OrientationDockBlock();

  virtual const QFont&   getFont() const override;
  virtual int            getPrecision() const override;
  virtual int            getFieldWidth() const override;
  virtual double         getScale() const override;
  virtual bool           unitVisible() const override;
  virtual const QString& getUnit() const override;

  virtual void setFont( const QFont& font ) override;
  virtual void setPrecision( const int precision ) override;
  virtual void setFieldWidth( const int fieldWidth ) override;
  virtual void setScale( const double scale ) override;
  virtual void setUnitVisible( const bool enabled ) override;
  virtual void setUnit( const QString& unit ) override;

public Q_SLOTS:
  void setName( const QString& name ) override;

  void setOrientation( ORIENTATION_SIGNATURE_SLOT );

  void setPose( POSE_SIGNATURE_SLOT );

public:
  ThreeValuesDock* widget = nullptr;

  RateLimiter rateLimiter;
};

class OrientationDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  OrientationDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {
    typeColor = TypeColor::Dock;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "OrientationDockBlock" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Display Docks" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Orientation Dock" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
