// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <vector>

class MeanMovingWindow {
public:
  void addValue( const double value );

  void setWindow( const std::size_t window );

  double getMean();

public:
  std::vector< double > values;
  std::size_t           window = 5;
  std::size_t           middle = 3;
};
