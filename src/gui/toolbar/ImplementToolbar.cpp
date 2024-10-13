// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ImplementToolbar.h"

#include "block/sectionControl/Implement.h"
#include "block/sectionControl/ImplementSection.h"

#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QPointer>
#include <QString>
#include <QToolButton>

ImplementToolbar::ImplementToolbar( Implement* implement, QWidget* parent ) : QGroupBox( parent ), implement( implement ) {
  setContentsMargins( 0, 0, 0, 0 );

  setMinimumSize( 60, 60 );
  buttonDefault = parent->palette();

  buttonYellow = parent->palette();
  buttonYellow.setColor( QPalette::Button, QColor( 190, 190, 0 ) );

  buttonGreen = parent->palette();
  buttonGreen.setColor( QPalette::Button, QColor( 90, 170, 90 ) );

  buttonRed = parent->palette();
  buttonRed.setColor( QPalette::Button, QColor( 255, 150, 150 ) );

  lbOn = new QLabel( QStringLiteral( "On" ), this );
  lbOn->setAlignment( Qt::AlignHCenter );

  lbOff = new QLabel( QStringLiteral( "Off" ), this );
  lbOff->setAlignment( Qt::AlignHCenter );

  pbAuto = new QToolButton( this );
  pbAuto->setText( QStringLiteral( "Auto" ) );
  pbAuto->setCheckable( true );
  pbAuto->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  pbAuto->setFocusPolicy( Qt::NoFocus );
  QObject::connect( pbAuto, SIGNAL( toggled( bool ) ), this, SLOT( autoToggled( bool ) ) );

  QObject::connect( implement, &Implement::implementChanged, this, &ImplementToolbar::implementChanged );
  QObject::connect( implement, &Implement::sectionsChanged, this, &ImplementToolbar::sectionsChanged );
}

QToolButton*
ImplementToolbar::addButtonToVector( const QString& name ) {
  auto* button = new QToolButton( this );
  button->setText( name );
  buttons.push_back( button );
  return button;
}

void
ImplementToolbar::addSection( const QString& name ) {
  if( horizontal ) {
    int currentCol = gridLayout->columnCount();
    gridLayout->addWidget( addButtonToVector( name ), 0, currentCol );
    gridLayout->addWidget( addButtonToVector( name ), 1, currentCol );
  } else {
    int currentRow = gridLayout->rowCount();
    gridLayout->addWidget( addButtonToVector( name ), currentRow, 0 );
    gridLayout->addWidget( addButtonToVector( name ), currentRow, 1 );
  }
}

void
ImplementToolbar::implementChanged( const QPointer< Implement >& ) {
  if( this != qobject_cast< ImplementToolbar* >( sender() ) ) {
    size_t numButtons = implement->sections.size() > 2 ? implement->sections.size() * 2 : ( implement->sections.size() - 1 ) * 2;

    static bool lastHorizontal = true;

    if( buttons.size() != numButtons || lastHorizontal != horizontal ) {
      // delete the old layout, as it takes ownership, remove the persistent labels and
      // button first
      if( gridLayout != nullptr ) {
        gridLayout->removeWidget( lbOn );
        gridLayout->removeWidget( lbOff );
        gridLayout->removeWidget( pbAuto );

        for( auto* button : std::as_const( buttons ) ) {
          gridLayout->removeWidget( button );
          button->setParent( this );
          button->deleteLater();
        }

        delete gridLayout;
      }

      buttons.clear();

      // create a new layout
      gridLayout = new QGridLayout( this );
      this->setLayout( gridLayout );

      if( horizontal ) {
        gridLayout->addWidget( lbOn, 0, 0 );
        gridLayout->addWidget( lbOff, 1, 0 );
      } else {
        gridLayout->addWidget( lbOn, 0, 0 );
        gridLayout->addWidget( lbOff, 0, 1 );
      }

      // only add buttons if there are sections present
      if( implement->sections.size() > 1 ) {
        // create the one button to control them all, but only if more than 1 section
        // present
        if( implement->sections.size() > 2 ) {
          addSection( QStringLiteral( "All" ) );
        }

        int buttonsToCreate = ( implement->sections.size() );

        for( int i = 1; i < buttonsToCreate; ++i ) {
          addSection( QString::number( i ) );
        }

        for( const auto& button : std::as_const( buttons ) ) {
          button->setCheckable( true );
          button->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
          button->setFocusPolicy( Qt::NoFocus );
          QObject::connect( button, SIGNAL( toggled( bool ) ), this, SLOT( forceOnOffToggled( bool ) ) );
        }
      }

      if( horizontal ) {
        gridLayout->addWidget( pbAuto, 0, gridLayout->columnCount(), 0, 1 );
      } else {
        gridLayout->addWidget( pbAuto, gridLayout->rowCount(), 0, 1, 0 );
      }

      lastHorizontal = horizontal;
    }
  }
}

void
ImplementToolbar::sectionsChanged() {
  //  if( this != qobject_cast<SectionControlToolbar*>( sender() ) ) {

  //  }
}

void
ImplementToolbar::forceOnOffToggled( bool checked ) {
  auto* clickedButton = qobject_cast< QToolButton* >( sender() );

  if( clickedButton != nullptr ) {
    int col;
    int row;
    int colSpan;
    int rowSpan;
    gridLayout->getItemPosition( gridLayout->indexOf( clickedButton ), &row, &col, &rowSpan, &colSpan );

    int  sectionIndex = 0;
    bool forceOn      = false;

    if( horizontal ) {
      sectionIndex = col;

      if( row == 0 ) {
        forceOn = true;
      }
    } else {
      sectionIndex = row;

      if( col == 0 ) {
        forceOn = true;
      }
    }

    if( implement->sections.size() > 2 ) {
      --sectionIndex;
    }

    const auto& section = implement->sections.at( sectionIndex );

    if( forceOn ) {
      section->setState( ImplementSection::State::ForceOn, checked );

      if( checked ) {
        clickedButton->setPalette( buttonGreen );
      } else {
        clickedButton->setPalette( buttonDefault );
      }
    } else {
      section->setState( ImplementSection::State::ForceOff, checked );

      if( checked ) {
        clickedButton->setPalette( buttonRed );
      } else {
        clickedButton->setPalette( buttonDefault );
      }
    }

    implement->emitSectionsChanged();
  }
}

void
ImplementToolbar::autoToggled( bool checked ) {
  auto* clickedButton = qobject_cast< QToolButton* >( sender() );

  if( clickedButton != nullptr ) {
    for( auto& section : implement->sections ) {
      section->setState( ImplementSection::State::Automatic, checked );
    }

    if( checked ) {
      clickedButton->setPalette( buttonYellow );
    } else {
      clickedButton->setPalette( buttonDefault );
    }

    implement->emitSectionsChanged();
  }
}

void
ImplementToolbar::resizeEvent( QResizeEvent* event ) {
  horizontal = ( event->size().width() > event->size().height() );
  implementChanged( implement );
}
