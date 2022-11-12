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

#include <QObject>

class MyMainWindow;
class XteDock;

#include "ValueDockBlockBase.h"
#include "block/BlockBase.h"

class XteDockBlock : public ValueDockBlockBase {
  Q_OBJECT

public:
  explicit XteDockBlock( const QString& uniqueName, MyMainWindow* mainWindow );

  ~XteDockBlock();

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

  void setXte( NUMBER_SIGNATURE_SLOT );

public:
  XteDock* widget = nullptr;
};

class XteDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  XteDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "XteDockBlock" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Display Docks" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "XTE Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;

public:
  static KDDockWidgets::DockWidget* firstDock;
};
