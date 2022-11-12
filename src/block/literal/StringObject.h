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

class StringBlockModel;

class StringObject : public BlockBase {
  Q_OBJECT

public:
  explicit StringObject() : BlockBase() {}

  void emitConfigSignals() override;

  void toJSON( QJsonObject& json ) override;
  void fromJSON( QJsonObject& json ) override;

Q_SIGNALS:
  void stringChanged( const QString& );

public:
  QString string;
};

class StringFactory : public BlockFactory {
  Q_OBJECT

public:
  StringFactory( QThread* thread, StringBlockModel* model ) : BlockFactory( thread ), model( model ) {}

  QString getNameOfFactory() override { return QStringLiteral( "String" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Literals" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  StringBlockModel* model = nullptr;
};
