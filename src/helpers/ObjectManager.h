// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <vector>

#include "block/BlockBase.h"

// https://en.wikipedia.org/wiki/Singleton_pattern
class ObjectsManager : public QObject {
  Q_OBJECT

public:
public:
  static ObjectsManager& instance() {
    if( nullptr == _instance )
      _instance = new ObjectsManager;
    return *_instance;
  }

  ObjectsManager& operator=( const ObjectsManager& ) = delete;

private:
  ObjectsManager()  = default;
  ~ObjectsManager() = default;
  static ObjectsManager* _instance;

signals:
  void objectsChanged();

public:
  std::vector< BlockBase* > objects;
};
