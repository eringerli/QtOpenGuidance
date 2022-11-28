// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class OpenSaveHelper : public QObject {
  Q_OBJECT
public:
  OpenSaveHelper() = delete;
  explicit OpenSaveHelper( QString title, QString filter, QWidget* mainWindow, QObject* parent = nullptr );

public Q_SLOTS:
  void open();
  void save();

Q_SIGNALS:
  void saveFile( QString fileName );
  void openFile( QString fileName );

public:
  QString  title;
  QString  filter;
  QWidget* mainWindow = nullptr;
};
