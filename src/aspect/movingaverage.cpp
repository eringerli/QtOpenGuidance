// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#include "movingaverage.h"

MovingAverage::MovingAverage( int samples )
  : m_maxSampleCount( samples )
  , m_samples( samples ) {
}

void MovingAverage::addSample( float sample ) {
  if( m_sampleCount == m_maxSampleCount ) {
    m_total -= m_samples.at( m_currentSample );
  } else {
    ++m_sampleCount;
  }

  m_samples[m_currentSample] = sample;
  m_total += sample;
  ++m_currentSample;

  if( m_currentSample >= m_maxSampleCount ) {
    m_currentSample = 0;
  }
}

float MovingAverage::average() const {
  return m_sampleCount ? m_total / static_cast<float>( m_sampleCount ) : 0.0f;
}
