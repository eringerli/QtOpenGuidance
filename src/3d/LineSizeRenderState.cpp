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

#include "LineSizeRenderState.h"

#include <Qt3DRender/QEffect>
#include <Qt3DRender/QLineWidth>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QTechnique>

LineSizeRenderState::LineSizeRenderState( Qt3DRender::QMaterial* material )
//      :material(material)
{
  // https://github.com/jclay/dev-journal/blob/master/draw-points-qt3d.md
  auto effect = material->effect();
  qDebug() << "effect" << effect;

  for( auto t : effect->techniques() ) {
    qDebug() << "techniques" << t;

    for( auto rp : t->renderPasses() ) {
      qDebug() << "renderPasses" << rp;
      auto lineSize = new Qt3DRender::QLineWidth();
      lineSize->setValue( 5.0f );
      lineSize->setSmooth( true );
      rp->addRenderState( lineSize );
      lineWidthRenderStates.push_back( lineSize );
    }
  }
}

LineSizeRenderState::~LineSizeRenderState() {
  for( auto lineSize : lineWidthRenderStates ) {
    lineSize->deleteLater();
  }
}

void
LineSizeRenderState::setLineWidth( float size ) {
  for( auto lineSize : lineWidthRenderStates ) {
    qDebug() << "setLineWidth" << lineSize << size;
    lineSize->setValue( size );
  }
}
