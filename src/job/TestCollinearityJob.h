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
