// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#include "fpsaspect.h"
#include "fpsmonitor.h"
#include "fpsmonitorbackend.h"
#include "updatefpsmonitorsjob.h"
#include <QDebug>

FpsAspect::FpsAspect( QObject* parent )
  : Qt3DCore::QAbstractAspect( parent )
  , m_updateMonitorsJob( new UpdateFpsMonitorsJob( this ) ) {
  // Register the mapper to handle creation, lookup, and destruction of backend nodes
  auto mapper = QSharedPointer<FpsMonitorMapper>::create( this );
  registerBackendType<FpsMonitor>( mapper );
}

void FpsAspect::addFpsMonitor( Qt3DCore::QNodeId id, FpsMonitorBackend* fpsMonitor ) {
  m_fpsMonitors.insert( id, fpsMonitor );
}

FpsMonitorBackend* FpsAspect::fpsMonitor( Qt3DCore::QNodeId id ) {
  return m_fpsMonitors.value( id, nullptr );
}

FpsMonitorBackend* FpsAspect::takeFpsMonitor( Qt3DCore::QNodeId id ) {
  return m_fpsMonitors.take( id );
}

QVector<Qt3DCore::QAspectJobPtr> FpsAspect::jobsToExecute( qint64 time ) {
  const auto dt = time - m_lastTime;
  m_lastTime = time;
  m_updateMonitorsJob->setFrameTimeDelta( dt );
  return { m_updateMonitorsJob };
}
