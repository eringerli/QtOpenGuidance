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
