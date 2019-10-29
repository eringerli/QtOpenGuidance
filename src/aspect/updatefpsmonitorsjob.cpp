// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#include "updatefpsmonitorsjob.h"
#include "fpsaspect.h"
#include "fpsmonitorbackend.h"

UpdateFpsMonitorsJob::UpdateFpsMonitorsJob( FpsAspect* aspect )
  : Qt3DCore::QAspectJob()
  , m_aspect( aspect ) {
  Q_ASSERT( aspect );
}

void UpdateFpsMonitorsJob::setFrameTimeDelta( qint64 dt ) {
  m_dt = dt;
}

void UpdateFpsMonitorsJob::run() {
  // Iterate over all the enabled fps monitors and add
  // the instantaneous fps value to their moving averages.
  const float fps = 1.0f / ( float( m_dt ) * 1.0e-9 );
  const auto& fpsMonitors = m_aspect->fpsMonitors();

  for( const auto fpsMonitor : fpsMonitors ) {
    // Don't process disabled monitors
    if( !fpsMonitor->isEnabled() )
      continue;

    fpsMonitor->addFpsSample( fps );
  }
}
