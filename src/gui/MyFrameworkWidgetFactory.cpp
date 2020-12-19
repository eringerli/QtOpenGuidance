/*
  This file is part of KDDockWidgets.

  Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MyFrameworkWidgetFactory.h"

#include <kddockwidgets/FrameworkWidgetFactory.h>
//#include <kddockwidgets/widgets/SeparatorWidget_p.h>
#include <QApplication>


class MyTitleBar : public KDDockWidgets::TitleBar {
    Q_OBJECT

  public:
    explicit MyTitleBar( KDDockWidgets::Frame* frame )
      : KDDockWidgets::TitleBar( frame ) {
      init();
    }

    explicit MyTitleBar( KDDockWidgets::FloatingWindow* fw )
      : KDDockWidgets::TitleBar( fw ) {
      init();

      setContentsMargins( 0, 0, 0, 0 );
    }

    void init() {
      QFontMetrics fm( qApp->font() );
      setFixedHeight( fm.height() + 0 );
    }

    void paintEvent( QPaintEvent* ) override {
      QPainter p( this );
      QFontMetrics fm( qApp->font() );
      p.setFont( qApp->font() );
      p.drawText( QPoint( 6, fm.ascent() ), title() );
    }
};

KDDockWidgets::TitleBar* CustomWidgetFactory::createTitleBar( KDDockWidgets::Frame* frame ) const {
  return new MyTitleBar( frame );
}

KDDockWidgets::TitleBar* CustomWidgetFactory::createTitleBar( KDDockWidgets::FloatingWindow* fw ) const {
  return new MyTitleBar( fw );
}

KDDockWidgets::Frame* CustomWidgetFactory::createCentralFrame( QWidgetOrQuick* parent ) const {
  return createFrame( parent, KDDockWidgets::FrameOptions() | KDDockWidgets::FrameOption_IsCentralFrame );
}
