// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QPainter>
#include <kddockwidgets/FrameworkWidgetFactory.h>
#include <kddockwidgets/private/TitleBar_p.h>

class CustomWidgetFactory : public KDDockWidgets::DefaultWidgetFactory {
public:
  CustomWidgetFactory() = default;

  KDDockWidgets::TitleBar* createTitleBar( KDDockWidgets::Frame* frame ) const override;
  KDDockWidgets::TitleBar* createTitleBar( KDDockWidgets::FloatingWindow* fw ) const override;

  Q_DISABLE_COPY( CustomWidgetFactory )
};
