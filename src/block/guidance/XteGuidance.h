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

#include <QObject>
#include <QVector3D>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

#include "kinematic/Plan.h"

// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

class XteGuidance : public BlockBase {
  Q_OBJECT

public:
  explicit XteGuidance() : BlockBase() {}

public Q_SLOTS:
  void setPose( POSE_SIGNATURE_SLOT );

  void setPlan( const Plan& plan );

  void emitConfigSignals() override;

Q_SIGNALS:
  void xteChanged( NUMBER_SIGNATURE_SIGNAL );
  void headingOfPathChanged( NUMBER_SIGNATURE_SIGNAL );
  void curvatureOfPathChanged( NUMBER_SIGNATURE_SIGNAL );
  void passNumberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  Plan plan;
};

class XteGuidanceFactory : public BlockFactory {
  Q_OBJECT

public:
  XteGuidanceFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Cross Track Error" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Guidance" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
