// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MyViewFactory.h"

#include <kddockwidgets/qtwidgets/views/Separator.h>
#include <kddockwidgets/qtwidgets/views/TitleBar.h>

#include <kddockwidgets/core/TitleBar.h>

#include <QApplication>
#include <QPainter>

class MyTitleBar : public KDDockWidgets::QtWidgets::TitleBar {
public:
  explicit MyTitleBar( KDDockWidgets::Core::TitleBar* controller, KDDockWidgets::Core::View* parent = nullptr )
      : KDDockWidgets::QtWidgets::TitleBar( controller, parent ), m_controller( controller ) {
    setFixedHeight( 60 );
  }

  ~MyTitleBar() override;

  void paintEvent( QPaintEvent* ) override {
    QPainter     p( this );
    QPen         pen( Qt::black );
    const QColor focusedBackgroundColor = Qt::yellow;
    const QColor backgroundColor        = focusedBackgroundColor.darker( 115 );
    QBrush       brush( m_controller->isFocused() ? focusedBackgroundColor : backgroundColor );
    pen.setWidth( 4 );
    p.setPen( pen );
    p.setBrush( brush );
    p.drawRect( rect().adjusted( 4, 4, -4, -4 ) );
    QFont f = qGuiApp->font();
    f.setPixelSize( 30 );
    f.setBold( true );
    p.setFont( f );
    p.drawText( QPoint( 10, 40 ), m_controller->title() );
  }

private:
  KDDockWidgets::Core::TitleBar* const m_controller;
};

MyTitleBar::~MyTitleBar() = default;

// Inheriting from SeparatorWidget instead of Separator as it handles moving and mouse cursor
// changing
class MySeparator : public KDDockWidgets::QtWidgets::Separator {
public:
  explicit MySeparator( KDDockWidgets::Core::Separator* controller, KDDockWidgets::Core::View* parent )
      : KDDockWidgets::QtWidgets::Separator( controller, parent ) {}

  ~MySeparator() override;

  void paintEvent( QPaintEvent* ) override {
    QPainter p( this );
    p.fillRect( QWidget::rect(), Qt::cyan );
  }
};

MySeparator::~MySeparator() = default;

KDDockWidgets::Core::View*
CustomWidgetFactory::createTitleBar( KDDockWidgets::Core::TitleBar* controller, KDDockWidgets::Core::View* parent ) const {
  // Feel free to return MyTitleBar_CSS here instead, but just for education purposes!
  return new MyTitleBar( controller, parent );
}

KDDockWidgets::Core::View*
CustomWidgetFactory::createSeparator( KDDockWidgets::Core::Separator* controller, KDDockWidgets::Core::View* parent ) const {
  return new MySeparator( controller, parent );
}

// #include "MyViewFactory.moc"
