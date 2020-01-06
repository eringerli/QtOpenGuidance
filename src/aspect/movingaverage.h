// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <QtCore/qvector.h>

class MovingAverage {
  public:
    explicit MovingAverage( int samples = 3 );

    void addSample( float sample );
    float average() const;

  private:
    int m_maxSampleCount = 0;
    int m_sampleCount = 0;
    int m_currentSample = 0;
    float m_total = 0;
    QVector<float> m_samples;
};

#endif // MOVINGAVERAGE_H
