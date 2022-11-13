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

#include "3d/qt3dForwards.h"

#include <memory>
#include <vector>

class LineSizeRenderState {
public:
  explicit LineSizeRenderState( Qt3DRender::QMaterial* material );

  ~LineSizeRenderState();

  void setLineWidth( float size );

private:
  //    Qt3DRender::QMaterial* material = nullptr;
  std::vector< Qt3DRender::QLineWidth* > lineWidthRenderStates;
};
