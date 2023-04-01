// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "kddockwidgets/KDDockWidgets.h"
#include <QWidget>

#include <QGroupBox>

class Implement;
class QGridLayout;
class QLabel;
class QToolButton;

class ImplementToolbar : public QGroupBox {
  Q_OBJECT

public:
  explicit ImplementToolbar( Implement* implement, QWidget* parent = nullptr );

Q_SIGNALS:

public Q_SLOTS:
  void sectionsChanged();
  void implementChanged( const QPointer< Implement >& );

private Q_SLOTS:
  void forceOnOffToggled( const bool );
  void autoToggled( const bool );

protected:
  void resizeEvent( QResizeEvent* event ) override;

private:
  QToolButton* addButtonToVector( const QString& name );
  void         addSection( const QString& name );

private:
  Implement* implement = nullptr;
  QPalette   buttonYellow, buttonRed, buttonGreen, buttonDefault;

  QGridLayout* gridLayout = nullptr;

  QLabel*                     lbOn   = nullptr;
  QLabel*                     lbOff  = nullptr;
  QToolButton*                pbAuto = nullptr;
  std::vector< QToolButton* > buttons;

  bool horizontal = true;
};
