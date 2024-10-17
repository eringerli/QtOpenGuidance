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
  explicit ValueDockBlockBase(
    QString uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString type, const BlockBase::TypeColor typeColor )
      : BlockBase( idHint, systemBlock, type, typeColor ) {
    dock = new KDDockWidgets::QtWidgets::DockWidget( uniqueName );
  }

  virtual ~ValueDockBlockBase() { dock->deleteLater(); }

  void toJSON( QJsonObject& json ) const override;
  void fromJSON( const QJsonObject& ) override;

public:
  virtual const QFont&   getFont() const       = 0;
  virtual int            getPrecision() const  = 0;
  virtual int            getFieldWidth() const = 0;
  virtual double         getScale() const      = 0;
  virtual bool           unitVisible() const   = 0;
  virtual const QString& getUnit() const       = 0;

  virtual void setFont( const QFont& )   = 0;
  virtual void setPrecision( int )       = 0;
  virtual void setFieldWidth( int )      = 0;
  virtual void setScale( double )        = 0;
  virtual void setUnitVisible( bool )    = 0;
  virtual void setUnit( const QString& ) = 0;

public Q_SLOTS:
  virtual void enable( const bool enable ) override;

public:
  KDDockWidgets::QtWidgets::DockWidget* dock = nullptr;

  static KDDockWidgets::QtWidgets::DockWidget* firstValueDock;
  static KDDockWidgets::QtWidgets::DockWidget* firstThreeValuesDock;
};
