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

#include "../gui/NumberBlockModel.h"

#pragma once

#include <QObject>

#include "BlockBase.h"

class ComparisonNotEqualTo : public BlockBase {
    Q_OBJECT

  public:
    explicit ComparisonNotEqualTo()
      : BlockBase() {}

    void emitConfigSignals() override {
      emit stateChanged( result );
    }

    void setValueA( double number ) {
      numberA = number;
      operation();
    }

    void setValueB( double number ) {
      numberB = number;
      operation();
    }

  private:
    void operation() {
      result = !qFuzzyCompare( numberA, numberB );
      emit stateChanged( result );
    }

  signals:
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

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new ComparisonNotEqualTo();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "A" ), QLatin1String( SLOT( setValueA( double ) ) ) );
      b->addPort( QStringLiteral( "!=" ), QLatin1String(), false, QNEPort::NoBullet );
      b->addInputPort( QStringLiteral( "B" ), QLatin1String( SLOT( setValueB( double ) ) ) );

      b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( stateChanged( bool ) ) ) );

      b->setBrush( arithmeticColor );

      return b;
    }
};
