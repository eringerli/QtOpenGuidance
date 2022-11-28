// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFlags>
#include <QObject>

#pragma once

class ImplementSection {
  Q_GADGET

public:
  ImplementSection() {}
  ImplementSection( const double overlapLeft, const double widthOfSection, const double overlapRight )
      : overlapLeft( overlapLeft ), widthOfSection( widthOfSection ), overlapRight( overlapRight ) {}

public:
  enum State { None = 0x0, ForceOn = 0x1, ForceOff = 0x2, Automatic = 0x4, AutomaticOn = 0x8, AutomaticOff = 0x10 };
  Q_DECLARE_FLAGS( States, State )
  Q_FLAG( States )

public:
  const States& state() { return stateOfSection; }

  void setState( const States states ) { stateOfSection = states; }

  void setState( const States states, const bool enabled ) {
    if( enabled ) {
      addState( states );
    } else {
      removeState( states );
    }
  }

  void addState( const States states ) { stateOfSection |= states; }

  void removeState( const States states ) { stateOfSection &= ~states; }

  bool isSectionOn() {
    if( stateOfSection.testFlag( State::ForceOff ) ) {
      return false;
    }

    if( stateOfSection.testFlag( State::ForceOn ) ) {
      return true;
    }

    if( stateOfSection.testFlag( State::Automatic ) ) {
      if( stateOfSection.testFlag( State::AutomaticOff ) ) {
        return false;
      }

      if( stateOfSection.testFlag( State::AutomaticOn ) ) {
        return true;
      }
    }

    return false;
  }

public:
  double overlapLeft    = 0;
  double widthOfSection = 0;
  double overlapRight   = 0;

private:
  States stateOfSection = State::None;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( ImplementSection::States )
