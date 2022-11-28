// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ThreadWeaver/QObjectDecorator>
#include <ThreadWeaver/ThreadWeaver>

#include <QObject>
#include <QPointer>

#include "helpers/cgalHelper.h"

class CultivatedAreaOptimitionController : public QObject {
  Q_OBJECT
public:
  CultivatedAreaOptimitionController() = delete;

  CultivatedAreaOptimitionController( std::shared_ptr< std::vector< Point_3 > >   pointsPointer,
                                      std::shared_ptr< std::vector< QVector3D > > normalsPointer );
  virtual ~CultivatedAreaOptimitionController();

  void run( const double maxDeviation );

  void stop();

private:
  std::shared_ptr< std::vector< Point_3 > >   pointsPointer;
  std::shared_ptr< std::vector< QVector3D > > normalsPointer;

  ThreadWeaver::JobPointer collection;

  std::shared_ptr< std::vector< Point_3 > > polyline_XY;
  std::shared_ptr< std::vector< Point_3 > > polyline_XZ;

  std::shared_ptr< std::vector< Point_3 > >   resultPoints;
  std::shared_ptr< std::vector< QVector3D > > resultNormals;

  double minDistanceSquaredXY = std::numeric_limits< double >::infinity();
  double minDistanceSquaredXZ = std::numeric_limits< double >::infinity();

Q_SIGNALS:
  void simplifyPolylineResult( std::shared_ptr< std::vector< Point_3 > >   resultPoints,
                               std::shared_ptr< std::vector< QVector3D > > resultNormals,
                               const double                                maxDeviation );
};
