// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "helpers/eigenHelper.h"

class MyMainWindow;
class ThreeValuesDock;

#include "ValueDockBlockBase.h"
#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"

class PositionDockBlock : public ValueDockBlockBase {
  Q_OBJECT

public:
  explicit PositionDockBlock( MyMainWindow*              mainWindow,
                              const QString&             uniqueName,
                              const BlockBaseId          idHint,
                              const bool                 systemBlock,
                              const QString              type,
                              const BlockBase::TypeColor typeColor );

  ~PositionDockBlock();

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

  void setPose( POSE_SIGNATURE_SLOT );

  void setVector( VECTOR_SIGNATURE_SLOT );
  void setWgs84( VECTOR_SIGNATURE_SLOT );

public:
  ThreeValuesDock* widget = nullptr;

  bool wgs84 = false;

  RateLimiter rateLimiter;
};

class PositionDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  PositionDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {
    typeColor = BlockBase::TypeColor::Dock;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "PositionDockBlock" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Display Docks" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Position Dock" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
