// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFlags>
#include <QObject>

#pragma once

class CalculationOption {
  Q_GADGET

public:
  enum Option {
    None = 0,

    NoOrientation = ( 1 << 1 ),

    /// only do numerical calulations, no graphical stuff
    NoGraphics = ( 1 << 2 ),
    /// only do numerical calulations, no planner stuff
    NoPlanner = ( 1 << 3 ),
    /// only do numerical calulations, no control stuff
    NoControl = ( 1 << 4 ),
    /// only do numerical calulations, no XTE stuff
    NoXte = ( 1 << 5 ),

    CalculateLocalOffsets = ( 1 << 6 ) | NoXte | NoOrientation | NoGraphics | NoControl,

    CalculateMpcUpdate = ( 1 << 7 ) | NoGraphics | NoPlanner | NoControl,

    RestoreStateToBeforeMpcCalculation = ( 1 << 8 ) | NoOrientation | NoGraphics | NoPlanner | NoControl | NoXte
  };
  Q_DECLARE_FLAGS( Options, Option )
  Q_FLAG( Options )
};

Q_DECLARE_OPERATORS_FOR_FLAGS( CalculationOption::Options )

Q_DECLARE_METATYPE( CalculationOption::Options );
