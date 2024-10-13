// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QStandardItemModel>

class FactoriesManager;

class FactoriesModel : public QStandardItemModel {
  Q_OBJECT

public:
  explicit FactoriesModel( QObject* parent = nullptr );

public Q_SLOTS:
  void resetModel();
};
