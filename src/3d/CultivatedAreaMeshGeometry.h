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

#include <QAttribute>
#include <QGeometry>
#include <Qt3DRender/QBuffer>

#include "../helpers/cgalHelper.h"

class CgalThread;

class CultivatedAreaMeshGeometry : public Qt3DRender::QGeometry {
    Q_OBJECT

  public:
    CultivatedAreaMeshGeometry( Qt3DCore::QNode* parent = nullptr );
    ~CultivatedAreaMeshGeometry();
    int vertexCount();

    void addPoints( const Point_2 pointLeft, const Point_2 pointRight );
    void addPointLeft( const Point_2 point );
    void addPointRight( const Point_2 point );

    void addTrackMeshGeometry( CultivatedAreaMeshGeometry* trackMeshGeometry );

    void optimise( CgalThread* thread );

  private:
    void addPointLeftWithoutUpdate( const Point_2 point );
    void addPointRightWithoutUpdate( const Point_2 point );
    void updateBuffers();

  Q_SIGNALS:
    void simplifyPolylineLeft( std::vector<Point_2>* pointsPointer, double maxDeviation );
    void simplifyPolylineRight( std::vector<Point_2>* pointsPointer, double maxDeviation );
    void vertexCountChanged( int );

  private Q_SLOTS:
    void simplifyPolylineResultLeft( std::vector<Point_2>* );
    void simplifyPolylineResultRight( std::vector<Point_2>* );

  private:
    Qt3DRender::QAttribute* m_positionAttribute = nullptr;
    Qt3DRender::QAttribute* m_normalAttribute = nullptr;
    Qt3DRender::QAttribute* m_tangentAttribute = nullptr;
    Qt3DRender::QAttribute* m_indexAttribute = nullptr;
    Qt3DRender::QBuffer* m_vertexBuffer = nullptr;
    Qt3DRender::QBuffer* m_indexBuffer = nullptr;

    std::vector<Point_2> trackPointsLeft;
    std::vector<Point_2> trackPointsRight;
    double maxDeviation = 0.003;

    bool waitForOptimition = false;
};
