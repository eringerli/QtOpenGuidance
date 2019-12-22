// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#ifndef FPSMONITOR_H
#define FPSMONITOR_H

#include <Qt3DCore/QComponent>

class FpsMonitor : public Qt3DCore::QComponent {
    Q_OBJECT
    Q_PROPERTY( int rollingMeanFrameCount READ rollingMeanFrameCount WRITE setRollingMeanFrameCount NOTIFY rollingMeanFrameCountChanged )
    Q_PROPERTY( float framesPerSecond READ framesPerSecond NOTIFY framesPerSecondChanged )

  public:
    explicit FpsMonitor( Qt3DCore::QNode* parent = nullptr );

    float framesPerSecond() const;
    int rollingMeanFrameCount() const;

  public slots:
    void setRollingMeanFrameCount( int rollingMeanFrameCount );

  signals:
    void framesPerSecondChanged( float framesPerSecond );
    void rollingMeanFrameCountChanged( int rollingMeanFrameCount );

  protected:
    void sceneChangeEvent( const Qt3DCore::QSceneChangePtr& change ) override;

  private:
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;

    float m_framesPerSecond = 0;
    int m_rollingMeanFrameCount = 5;
};

struct FpsMonitorData {
  int rollingMeanFrameCount;
};

#endif // FPSMONITOR_H
