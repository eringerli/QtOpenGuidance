// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
