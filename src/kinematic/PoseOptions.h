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

#include <QObject>
#include <QFlags>

#ifndef POSEOPTIONS_H
#define POSEOPTIONS_H

class PoseOption {
    Q_GADGET

  public:
    enum Option {
      NoOptions = 0x0,
      CalculateLocalOffsets = 0x1,
      CalculateWithoutOrientation = 0x2,
      CalculateWithoutTiling = 0x4
    };
    Q_DECLARE_FLAGS( Options, Option )
    Q_FLAG( Options )
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PoseOption::Options )

#endif // POSEOPTIONS_H
