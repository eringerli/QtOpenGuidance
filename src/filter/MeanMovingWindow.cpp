// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MeanMovingWindow.h"

#include <vector>

void
MeanMovingWindow::addValue( const double value ) {
  values.push_back( value );

  if( values.size() > window ) {
    values.erase( values.begin() );
  }
}

void
MeanMovingWindow::setWindow( const std::size_t window ) {
  this->window = window;
  this->middle = window / 2;
}

double
MeanMovingWindow::getMean() {
  if( values.size() > middle ) {
    std::vector< double > copy( values );

    auto middleIterator = values.begin() + middle;

    std::nth_element( values.begin(), middleIterator, values.end() );

    return *middleIterator;
  } else {
    return 0;
  }
}
