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

#include "block/BlockBase.h"
#include "PlotDockBlockBase.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

class OrientationPlotDockBlock : public PlotDockBlockBase {
    Q_OBJECT

  public:
    explicit OrientationPlotDockBlock( const QString& uniqueName,
                                       MyMainWindow* mainWindow );

  public Q_SLOTS:
    void setOrientation( const Eigen::Quaterniond& orientation );

    void setPose( const Eigen::Vector3d&, const Eigen::Quaterniond& orientation, const PoseOption::Options& );

  private:
    void rescale();
};

class OrientationPlotDockBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    OrientationPlotDockBlockFactory( MyMainWindow* mainWindow,
                                     KDDockWidgets::Location location,
                                     QMenu* menu )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "OrientationPlotDockBlock" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Plots" );
    }

    QString getPrettyNameOfFactory() override {
      return QStringLiteral( "Orientation Plot Dock" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
};
