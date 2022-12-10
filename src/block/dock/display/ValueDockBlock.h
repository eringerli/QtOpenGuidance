// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MyMainWindow;
class ValueDock;

#include "ValueDockBlockBase.h"
#include "block/BlockBase.h"

class ValueDockBlock : public ValueDockBlockBase {
  Q_OBJECT

public:
  explicit ValueDockBlock( const QString& uniqueName, MyMainWindow* mainWindow );

  ~ValueDockBlock();

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

  void setValue( NUMBER_SIGNATURE_SLOT );

public:
  ValueDock* widget = nullptr;
};

class ValueDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  ValueDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ValueDockBlock" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Display Docks" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Value Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
