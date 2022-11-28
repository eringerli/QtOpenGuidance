// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ThreadWeaver/ThreadWeaver>

#include "kinematic/cgalKernel.h"

class ConnectPointsJob : public ThreadWeaver::Job {
public:
  ConnectPointsJob() = delete;
  ConnectPointsJob( std::shared_ptr< std::vector< Point_2 > > pointsPointer, double distanceBetweenConnectPoints );
  ~ConnectPointsJob() { std::cout << "ConnectPointsJob::~ConnectPointsJob" << std::endl; }

protected:
  virtual void run( ThreadWeaver::JobPointer, ThreadWeaver::Thread* ) override;

private:
  std::shared_ptr< std::vector< Point_2 > > pointsPointer                = nullptr;
  double                                    distanceBetweenConnectPoints = 1;
};
