// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#ifndef FPSMONITORBACKEND_H
#define FPSMONITORBACKEND_H

#include <Qt3DCore/QBackendNode>
#include "movingaverage.h"

class FpsMonitorBackend : public Qt3DCore::QBackendNode {
  public:
    FpsMonitorBackend();

    void addFpsSample( float fpsSample );

  protected:
    void sceneChangeEvent( const Qt3DCore::QSceneChangePtr& e ) override;

  private:
    void initializeFromPeer( const Qt3DCore::QNodeCreatedChangeBasePtr& change ) override;

    int m_rollingMeanFrameCount;
    MovingAverage m_average;
};


class FpsAspect;

class FpsMonitorMapper : public Qt3DCore::QBackendNodeMapper {
  public:
    explicit FpsMonitorMapper( FpsAspect* aspect );

    Qt3DCore::QBackendNode* create( const Qt3DCore::QNodeCreatedChangeBasePtr& change ) const override;
    Qt3DCore::QBackendNode* get( Qt3DCore::QNodeId id ) const override;
    void destroy( Qt3DCore::QNodeId id ) const override;

  private:
    FpsAspect* m_aspect;
};

#endif // FPSMONITORBACKEND_H
