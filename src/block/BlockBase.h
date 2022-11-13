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

#include <QLatin1String>
#include <QObject>
#include <QString>
#include <QStringLiteral>

#include "kinematic/CalculationOptions.h"

#include "helpers/signatures.h"

#include "CompressorApplication.h"

class QJsonObject;
class QComboBox;
class QTreeWidget;
class QGraphicsScene;

class QNEBlock;
class QNEPort;

class BlockBase : public QObject {
  Q_OBJECT

public:
  BlockBase() {}

  virtual void emitConfigSignals() {}

  virtual void toJSON( QJsonObject& ) {}
  virtual void fromJSON( QJsonObject& ) {}

  virtual void setName( const QString& ) {}
};

class BlockFactory;

class BlockFactory : public QObject {
  Q_OBJECT

public:
  BlockFactory() = delete;
  BlockFactory( QThread* thread ) : thread( thread ) {}
  ~BlockFactory() {}

  virtual QString getNameOfFactory() = 0;

  virtual QString getPrettyNameOfFactory() { return getNameOfFactory(); }

  virtual QString getCategoryOfFactory() = 0;

  void addToCombobox( QComboBox* combobox );

  void addToTreeWidget( QTreeWidget* treeWidget );

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) = 0;

  QNEBlock* createBaseBlock( QGraphicsScene* scene, BlockBase* obj, int id, bool systemBlock = false );

  static bool isIdUnique( QGraphicsScene* scene, int id );

public:
  QThread* thread = nullptr;

protected:
  static const QColor modelColor;
  static const QColor dockColor;
  static const QColor inputDockColor;
  static const QColor parserColor;
  static const QColor valueColor;
  static const QColor inputOutputColor;
  static const QColor converterColor;
  static const QColor arithmeticColor;
};
