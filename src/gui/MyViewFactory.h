// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <kddockwidgets/qtwidgets/ViewFactory.h>

class CustomWidgetFactory : public KDDockWidgets::QtWidgets::ViewFactory {
  Q_OBJECT
public:
  KDDockWidgets::Core::View* createTitleBar( KDDockWidgets::Core::TitleBar*, KDDockWidgets::Core::View* parent ) const override;
  KDDockWidgets::Core::View* createSeparator( KDDockWidgets::Core::Separator*, KDDockWidgets::Core::View* parent = nullptr ) const override;
};
