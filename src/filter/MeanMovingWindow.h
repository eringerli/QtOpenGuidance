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

#pragma once

#include <vector>
#include <algorithm>

class MeanMovingWindow {
  public:
    MeanMovingWindow();

    void addValue( const double value ) {
      values.push_back( value );

      if( values.size() > window ) {
        values.erase( values.begin() );
      }
    }

    void setWindow( const std::size_t window ) {
      this->window = window;
      this->middle = window / 2;
    }

    double getMean() {
      if( values.size() > middle ) {
        std::vector<double> copy( values );

        auto middleIterator = values.begin() + middle;

        std::nth_element( values.begin(), middleIterator, values.end() );

        return *middleIterator;
      } else {
        return 0;
      }
    }

  public:
    std::vector<double> values;
    std::size_t window = 5;
    std::size_t middle = 3;
};
