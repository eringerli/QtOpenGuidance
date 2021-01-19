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

#include "block/BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/eigenHelper.h"
#include "helpers/GeographicConvertionWrapper.h"

#include <QDebug>

class TransverseMercatorConverter : public BlockBase {
    Q_OBJECT

  public:
    explicit TransverseMercatorConverter( GeographicConvertionWrapper* tmw )
      : BlockBase(),
        tmw( tmw ) {}

  public Q_SLOTS:
    void setWGS84Position( const Eigen::Vector3d& position );

  Q_SIGNALS:
    void positionChanged( const Eigen::Vector3d& );

  public:
    virtual void emitConfigSignals() override;

  public:
    GeographicConvertionWrapper* tmw = nullptr;
};

class TransverseMercatorConverterFactory : public BlockFactory {
    Q_OBJECT

  public:
    TransverseMercatorConverterFactory( GeographicConvertionWrapper* tmw )
      : BlockFactory(),
        tmw( tmw ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Transverse Mercator" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Calculations" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

  private:
    GeographicConvertionWrapper* tmw = nullptr;
};
