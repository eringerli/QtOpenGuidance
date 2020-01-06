// Copyright( C ) 2019 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "SectionControlToolbar.h"

#include "../block/Implement.h"

#include <QString>

SectionControlToolbar::SectionControlToolbar( Implement* implement, QWidget* parent )
  : QGroupBox( parent ),
    implement( implement ) {

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

  QObject::connect( implement, &Implement::implementChanged, this, &SectionControlToolbar::implementChanged );
  QObject::connect( implement, &Implement::sectionsChanged, this, &SectionControlToolbar::sectionsChanged );
}

QToolButton* SectionControlToolbar::addButtonToVector( const QString& name ) {
  auto* button = new QToolButton( this );
  button->setText( name );
  buttons.push_back( button );
  return button;
}

void SectionControlToolbar::addSection( const QString& name ) {
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

void SectionControlToolbar::implementChanged( const QPointer<Implement>& ) {
  if( this != qobject_cast<SectionControlToolbar*>( sender() ) ) {
    size_t numButtons = implement->sections.size() > 2 ?
                        implement->sections.size() * 2 :
                        ( implement->sections.size() - 1 ) * 2;

    static bool lastHorizontal = true;

    if( buttons.size() != numButtons || lastHorizontal != horizontal ) {

      // delete the old layout, as it takes ownership, remove the persistent labels and button first
      if( gridLayout != nullptr ) {
        gridLayout->removeWidget( lbOn );
        gridLayout->removeWidget( lbOff );
        gridLayout->removeWidget( pbAuto );

        for( auto button : qAsConst( buttons ) ) {
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
        // create the one button to control them all, but only if more than 1 section present
        if( implement->sections.size() > 2 ) {
          addSection( QStringLiteral( "All" ) );
        }

        int buttonsToCreate = ( implement->sections.size() );

        for( int i = 1; i < buttonsToCreate; ++i ) {
          addSection( QString::number( i ) );
        }

        for( const auto& button : qAsConst( buttons ) ) {
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

void SectionControlToolbar::sectionsChanged() {
//  if( this != qobject_cast<SectionControlToolbar*>( sender() ) ) {

//  }
}

void SectionControlToolbar::setDockLocation( Qt::DockWidgetArea area ) {
  if( area == Qt::NoDockWidgetArea ) {
    return;
  }

  horizontal = !( area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea );

  implementChanged( implement );
}

void SectionControlToolbar::forceOnOffToggled( bool checked ) {
  auto* clickedButton = qobject_cast<QToolButton*>( sender() );

  if( clickedButton != nullptr ) {
    int col;
    int row;
    int colSpan;
    int rowSpan;
    gridLayout->getItemPosition( gridLayout->indexOf( clickedButton ), &row, &col, &rowSpan, &colSpan );

    int sectionIndex = 0;
    bool forceOn = false;

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

void SectionControlToolbar::autoToggled( bool checked ) {
  auto* clickedButton = qobject_cast<QToolButton*>( sender() );

  if( clickedButton != nullptr ) {
    const auto& section = implement->sections[0];
    section->setState( ImplementSection::State::Automatic, checked );

    if( checked ) {
      clickedButton->setPalette( buttonYellow );
    } else {
      clickedButton->setPalette( buttonDefault );
    }

    implement->emitSectionsChanged();
  }
}
