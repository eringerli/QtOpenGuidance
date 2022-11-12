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

#include <ThreadWeaver/ThreadWeaver>

#include <QObject>

#include "helpers/cgalHelper.h"

class PlanOptimitionController : public QObject {
  Q_OBJECT
public:
  PlanOptimitionController() = delete;

  PlanOptimitionController( std::shared_ptr< std::vector< Point_2 > > pointsPointer, const double maxDeviation );
  virtual ~PlanOptimitionController();

  void run();

  void stop();

private:
  std::shared_ptr< std::vector< Point_2 > > pointsPointer;

  ThreadWeaver::JobPointer job;

  std::shared_ptr< std::vector< Point_2 > > resultPoints;

  double maxDeviation = 0.1;

Q_SIGNALS:
  void simplifyPolylineResult( std::shared_ptr< std::vector< Point_2 > > resultPoints );
};
