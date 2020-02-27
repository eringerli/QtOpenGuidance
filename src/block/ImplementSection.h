// Copyright( C ) 2020 Christian Riggenbach
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

#include <QObject>

#pragma once

class ImplementSection {
    Q_GADGET

  public:
    ImplementSection() {}
    ImplementSection( double overlapLeft, double widthOfSection, double overlapRight )
      : overlapLeft( overlapLeft ), widthOfSection( widthOfSection ), overlapRight( overlapRight ) {}

  public:
    enum State {
      None = 0x0,
      ForceOn = 0x1,
      ForceOff = 0x2,
      Automatic = 0x4,
      AutomaticOn = 0x8,
      AutomaticOff = 0x10
    };
    Q_DECLARE_FLAGS( States, State )
    Q_FLAG( States )

  public:
    const States& state() {
      return stateOfSection;
    }

    void setState( States states ) {
      stateOfSection = states;
    }

    void setState( States states, bool enabled ) {
      if( enabled ) {
        addState( states );
      } else {
        removeState( states );
      }
    }

    void addState( States states ) {
      stateOfSection |= states;
    }

    void removeState( States states ) {
      stateOfSection &= ~states;
    }

    bool isSectionOn() {
      if( stateOfSection.testFlag( State::ForceOff ) ) {
        return false;
      }

      if( stateOfSection.testFlag( State::ForceOn ) ) {
        return true;
      }

      if( stateOfSection.testFlag( State::Automatic ) ) {
        if( stateOfSection.testFlag( State::AutomaticOn ) ) {
          return true;
        }
      }

      return false;
    }

  public:
    double overlapLeft = 0;
    double widthOfSection = 0;
    double overlapRight = 0;

  private:
    States stateOfSection = State::None;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( ImplementSection::States )

