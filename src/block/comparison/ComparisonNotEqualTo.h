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

#include "gui/model/NumberBlockModel.h"

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class ComparisonNotEqualTo : public BlockBase {
    Q_OBJECT

  public:
    explicit ComparisonNotEqualTo()
      : BlockBase() {}

    void emitConfigSignals() override;

    void setValueA( double number );
    void setValueB( double number );

  private:
    void operation();

  Q_SIGNALS:
    void stateChanged( bool );

  public:
    double numberA = 0;
    double numberB = 0;
    bool result = false;
};

class ComparisonNotEqualToFactory : public BlockFactory {
    Q_OBJECT

  public:
    ComparisonNotEqualToFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "ComparisonNotEqualTo" );
    }

    QString getPrettyNameOfFactory() override {
      return QStringLiteral( "Comparison A != B" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Comparison" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
