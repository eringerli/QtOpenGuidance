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

#include <QPointer>
#include <Qt3DCore/QGeometry>

#include "helpers/cgalHelper.h"

#include "job/CultivatedAreaOptimitionController.h"

class CgalThread;

class CultivatedAreaMeshGeometry : public Qt3DCore::QGeometry {
  Q_OBJECT

public:
  CultivatedAreaMeshGeometry( Qt3DCore::QNode* parent = nullptr );
  ~CultivatedAreaMeshGeometry();
  int vertexCount();

  void addPoints( const Point_3 pointLeft, const Point_3 pointRight, const QVector3D normalVector );
  void addPointLeft( const Point_3 point, const QVector3D normalVector );
  void addPointRight( const Point_3 point, const QVector3D normalVector );

  void clear();

  void addTrackMeshGeometry( CultivatedAreaMeshGeometry* trackMeshGeometry );

  void optimise();

  size_t numPoints() const;

private:
  void addPointLeftWithoutUpdate( const Point_3 point, const QVector3D normalVector );
  void addPointRightWithoutUpdate( const Point_3 point, const QVector3D normalVector );
  void updateBuffers();

Q_SIGNALS:
  void simplifyPolylineLeft( std::vector< Point_3 >* pointsPointer, double maxDeviation );
  void simplifyPolylineRight( std::vector< Point_3 >* pointsPointer, double maxDeviation );

public Q_SLOTS:
  void simplifyPolylineResultLeft( std::shared_ptr< std::vector< Point_3 > >   resultPoints,
                                   std::shared_ptr< std::vector< QVector3D > > resultNormals,
                                   const double                                maxDeviation );
  void simplifyPolylineResultRight( std::shared_ptr< std::vector< Point_3 > >   resultPoints,
                                    std::shared_ptr< std::vector< QVector3D > > resultNormals,
                                    const double                                maxDeviation );

private:
  Qt3DCore::QAttribute* m_positionAttribute = nullptr;
  Qt3DCore::QAttribute* m_normalAttribute   = nullptr;
  Qt3DCore::QAttribute* m_tangentAttribute  = nullptr;
  Qt3DCore::QAttribute* m_indexAttribute    = nullptr;
  Qt3DCore::QBuffer*    m_vertexBuffer      = nullptr;
  Qt3DCore::QBuffer*    m_indexBuffer       = nullptr;

  std::shared_ptr< std::vector< Point_3 > >   trackPointsLeft;
  std::shared_ptr< std::vector< Point_3 > >   trackPointsRight;
  std::shared_ptr< std::vector< QVector3D > > trackNormalsLeft;
  std::shared_ptr< std::vector< QVector3D > > trackNormalsRight;
  double                                      maxDeviation = 0.1;

  bool waitForOptimition = false;

  QPointer< CultivatedAreaOptimitionController > cultivatedAreaOptimitionControllerLeft;
  QPointer< CultivatedAreaOptimitionController > cultivatedAreaOptimitionControllerRight;
};
