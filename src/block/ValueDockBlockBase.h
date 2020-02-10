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

#ifndef VALUEDOCKBLOCKBASE_H
#define VALUEDOCKBLOCKBASE_H

#include <QObject>
#include <QDockWidget>
#include <QSizePolicy>
#include <QMenu>

#include "../gui/MainWindow.h"

#include "BlockBase.h"

class ValueDockBlockBase : public BlockBase {
    Q_OBJECT

  public:
    explicit ValueDockBlockBase( MainWindow* mainWindow )
      : BlockBase() {
      dock = new QDockWidget( mainWindow );
    }

    virtual ~ValueDockBlockBase() {
      dock->deleteLater();
    }

  public:
    virtual const QFont& getFont() = 0;
    virtual int getPrecision() = 0;
    virtual int getFieldWidth() = 0;
    virtual double getScale() = 0;
    virtual bool captionEnabled() = 0;

    virtual void setFont( const QFont& ) = 0;
    virtual void setPrecision( int ) = 0;
    virtual void setFieldWidth( int ) = 0;
    virtual void setScale( double ) = 0;
    virtual void setCaptionEnabled( bool ) = 0;

  public slots:

  public:
    QDockWidget* dock = nullptr;
    QAction* action = nullptr;
};

#endif // VALUEDOCKBLOCKBASE_H
