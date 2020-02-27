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

#include "../cgalKernel.h"
#include "PathPrimitive.h"

class Plan {
  public:
    enum class Type : uint8_t {
      Lines,
      Segments,
      Circles,
      Mixed = 100
    };

    Plan() {
      plan = std::make_shared<std::vector<std::shared_ptr<PathPrimitive>>>();
    }


  public:
    Type type = Type::Mixed;
    std::shared_ptr<std::vector<std::shared_ptr<PathPrimitive>>> plan;
};

Q_DECLARE_METATYPE( Plan )

