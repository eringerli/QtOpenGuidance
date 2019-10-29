// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#ifndef FPSASPECT_H
#define FPSASPECT_H

#include <Qt3DCore/QAbstractAspect>
#include "updatefpsmonitorsjob.h"

class FpsMonitorBackend;

class FpsAspect : public Qt3DCore::QAbstractAspect {
    Q_OBJECT
  public:
    explicit FpsAspect( QObject* parent = nullptr );

    void addFpsMonitor( Qt3DCore::QNodeId id, FpsMonitorBackend* fpsMonitor );
    FpsMonitorBackend* fpsMonitor( Qt3DCore::QNodeId id );
    FpsMonitorBackend* takeFpsMonitor( Qt3DCore::QNodeId id );

    const QHash<Qt3DCore::QNodeId, FpsMonitorBackend*>& fpsMonitors() const {
      return m_fpsMonitors;
    }

  protected:
    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute( qint64 time ) override;

  private:
    QHash<Qt3DCore::QNodeId, FpsMonitorBackend*> m_fpsMonitors;
    UpdateFpsMonitorsJobPtr m_updateMonitorsJob;
    qint64 m_lastTime;
};

#endif // FPSASPECT_H
