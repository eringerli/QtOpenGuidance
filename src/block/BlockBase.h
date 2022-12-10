// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QLatin1String>
#include <QObject>
#include <QString>
#include <QStringLiteral>

#include "kinematic/CalculationOptions.h"

#include "helpers/signatures.h"

#include "qjsonobject.h"

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

  virtual QJsonObject toJSON() { return QJsonObject(); }
  virtual void        fromJSON( QJsonObject& ) {}

  virtual void setName( const QString& ) {}
};

class BlockFactory;

class BlockFactory : public QObject {
  Q_OBJECT

public:
  BlockFactory() = delete;
  BlockFactory( QThread* thread, bool systemBlock ) : thread( thread ), systemBlock( systemBlock ) {}
  ~BlockFactory() {}

  virtual QString getNameOfFactory() = 0;

  virtual QString getPrettyNameOfFactory() { return getNameOfFactory(); }

  virtual QString getCategoryOfFactory() = 0;

  void addToCombobox( QComboBox* combobox );

  void addToTreeWidget( QTreeWidget* treeWidget );

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) = 0;

  QNEBlock* createBaseBlock( QGraphicsScene* scene, BlockBase* obj, int id );

  static bool isIdUnique( QGraphicsScene* scene, int id );

public:
  QThread* thread      = nullptr;
  bool     systemBlock = false;

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
