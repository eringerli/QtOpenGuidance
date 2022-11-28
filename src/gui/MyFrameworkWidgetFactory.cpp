// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MyFrameworkWidgetFactory.h"

#include <kddockwidgets/FrameworkWidgetFactory.h>
//#include <kddockwidgets/widgets/SeparatorWidget_p.h>
#include <QApplication>

class MyTitleBar : public KDDockWidgets::TitleBar {
  Q_OBJECT

public:
  explicit MyTitleBar( KDDockWidgets::Frame* frame ) : KDDockWidgets::TitleBar( frame ) { init(); }

  explicit MyTitleBar( KDDockWidgets::FloatingWindow* fw ) : KDDockWidgets::TitleBar( fw ) {
    init();

    setContentsMargins( 0, 0, 0, 0 );
  }

  void init() {
    QFontMetrics fm( qApp->font() );
    setFixedHeight( fm.height() + 0 );
  }

  void paintEvent( QPaintEvent* ) override {
    QPainter     p( this );
    QFontMetrics fm( qApp->font() );
    p.setFont( qApp->font() );
    p.drawText( QPoint( 6, fm.ascent() ), title() );
  }
};

KDDockWidgets::TitleBar*
CustomWidgetFactory::createTitleBar( KDDockWidgets::Frame* frame ) const {
  return new MyTitleBar( frame );
}

KDDockWidgets::TitleBar*
CustomWidgetFactory::createTitleBar( KDDockWidgets::FloatingWindow* fw ) const {
  return new MyTitleBar( fw );
}

KDDockWidgets::Frame*
CustomWidgetFactory::createCentralFrame( KDDockWidgets::QWidgetOrQuick* parent ) const {
  return createFrame( parent, KDDockWidgets::FrameOptions() | KDDockWidgets::FrameOption_IsCentralFrame );
}

#include "MyFrameworkWidgetFactory.moc"
