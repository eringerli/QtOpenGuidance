// Copyright( C ) 2021 Christian Riggenbach
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

#include <ThreadWeaver/QObjectDecorator>
#include <ThreadWeaver/ThreadWeaver>

#include <QObject>
#include <QPointer>

#include "helpers/cgalHelper.h"

class CultivatedAreaOptimitionController : public QObject {
  Q_OBJECT
public:
  CultivatedAreaOptimitionController() = delete;

  CultivatedAreaOptimitionController( std::shared_ptr< std::vector< Point_3 > >   pointsPointer,
                                      std::shared_ptr< std::vector< QVector3D > > normalsPointer );
  virtual ~CultivatedAreaOptimitionController();

  void run( const double maxDeviation );

  void stop();

private:
  std::shared_ptr< std::vector< Point_3 > >   pointsPointer;
  std::shared_ptr< std::vector< QVector3D > > normalsPointer;

  ThreadWeaver::JobPointer collection;

  std::shared_ptr< std::vector< Point_3 > > polyline_XY;
  std::shared_ptr< std::vector< Point_3 > > polyline_XZ;

  std::shared_ptr< std::vector< Point_3 > >   resultPoints;
  std::shared_ptr< std::vector< QVector3D > > resultNormals;

  double minDistanceSquaredXY = std::numeric_limits< double >::infinity();
  double minDistanceSquaredXZ = std::numeric_limits< double >::infinity();

Q_SIGNALS:
  void simplifyPolylineResult( std::shared_ptr< std::vector< Point_3 > >   resultPoints,
                               std::shared_ptr< std::vector< QVector3D > > resultNormals,
                               const double                                maxDeviation );
};
