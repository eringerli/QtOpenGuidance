// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ThreadWeaver/ThreadWeaver>

#include "kinematic/cgalKernel.h"

class TestCollinearityJob : public ThreadWeaver::Job {
public:
  TestCollinearityJob() = delete;
  TestCollinearityJob( std::shared_ptr< std::vector< Point_2 > > pointsPointer );
  ~TestCollinearityJob() { std::cout << "TestCollinearityJob::~~~TestCollinearityJob " << this << std::endl; }

protected:
  virtual void run( ThreadWeaver::JobPointer, ThreadWeaver::Thread* ) override;

private:
  std::shared_ptr< std::vector< Point_2 > > pointsPointer = nullptr;
  bool                                      collinear     = true;
};
