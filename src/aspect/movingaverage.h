// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <QtCore/qvector.h>

class MovingAverage {
  public:
    explicit MovingAverage( unsigned int samples = 3 );

    void addSample( float sample );
    float average() const;

  private:
    unsigned int m_maxSampleCount;
    unsigned int m_sampleCount;
    unsigned int m_currentSample;
    float m_total;
    QVector<float> m_samples;
};

#endif // MOVINGAVERAGE_H
