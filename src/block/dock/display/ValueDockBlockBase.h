// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>

class ValueDockBlockBase : public BlockBase {
  Q_OBJECT

public:
  explicit ValueDockBlockBase( const QString& uniqueName ) : BlockBase() { dock = new KDDockWidgets::DockWidget( uniqueName ); }

  virtual ~ValueDockBlockBase() { dock->deleteLater(); }

  QJsonObject toJSON() override;
  void        fromJSON( QJsonObject& ) override;

public:
  virtual const QFont&   getFont()       = 0;
  virtual int            getPrecision()  = 0;
  virtual int            getFieldWidth() = 0;
  virtual double         getScale()      = 0;
  virtual bool           unitVisible()   = 0;
  virtual const QString& getUnit()       = 0;

  virtual void setFont( const QFont& )   = 0;
  virtual void setPrecision( int )       = 0;
  virtual void setFieldWidth( int )      = 0;
  virtual void setScale( double )        = 0;
  virtual void setUnitVisible( bool )    = 0;
  virtual void setUnit( const QString& ) = 0;

public Q_SLOTS:

public:
  KDDockWidgets::DockWidget* dock = nullptr;

  static KDDockWidgets::DockWidget* firstValueDock;
  static KDDockWidgets::DockWidget* firstThreeValuesDock;
};
