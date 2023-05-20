// MIT License
//
// Copyright (c) 2020 Christian Riggenbach
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

template< typename T >
class SchmittTrigger {
public:
  explicit SchmittTrigger() { calculateThresholds(); }

  explicit SchmittTrigger( const T hysteresis, const T threshold ) : hysteresis( hysteresis ), threshold( threshold ) {
    calculateThresholds();
  }

  bool trigger( const T value ) {
    if( upperLimit ) {
      if( value < lowerThreshold ) {
        upperLimit = false;
        return false;
      } else {
        return true;
      }
    } else {
      if( value > upperThreshold ) {
        upperLimit = true;
        return true;
      } else {
        return false;
      }
    }
  }

  void setHysteresis( const T hysteresis ) {
    this->hysteresis = hysteresis;

    calculateThresholds();
  }

  void setThreshold( const T threshold ) {
    this->threshold = threshold;

    calculateThresholds();
  }

  void setUpperThreshold( const T threshold ) {
    if( threshold > lowerThreshold ) {
      upperThreshold = threshold;
    }
  }

  void setLowerThreshold( const T threshold ) {
    if( threshold < upperThreshold ) {
      lowerThreshold = threshold;
    }
  }

private:
  void calculateThresholds() {
    lowerThreshold = threshold - ( hysteresis / 2 );
    upperThreshold = threshold + ( hysteresis / 2 );
  }

public:
  bool upperLimit = false;
  T    hysteresis = 4;
  T    threshold  = 5;

  T lowerThreshold = 0;
  T upperThreshold = 0;
};
