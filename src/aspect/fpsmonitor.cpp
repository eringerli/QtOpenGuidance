// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#include "fpsmonitor.h"
#include <Qt3DCore/qpropertyupdatedchange.h>

FpsMonitor::FpsMonitor( Qt3DCore::QNode* parent )
  : Qt3DCore::QComponent( parent ) {
}

float FpsMonitor::framesPerSecond() const {
  return m_framesPerSecond;
}

int FpsMonitor::rollingMeanFrameCount() const {
  return m_rollingMeanFrameCount;
}

void FpsMonitor::setRollingMeanFrameCount( int rollingMeanFrameCount ) {
  if( m_rollingMeanFrameCount == rollingMeanFrameCount )
    return;

  m_rollingMeanFrameCount = rollingMeanFrameCount;
  emit rollingMeanFrameCountChanged( m_rollingMeanFrameCount );
}

void FpsMonitor::sceneChangeEvent( const Qt3DCore::QSceneChangePtr& change ) {
  if( change->type() == Qt3DCore::PropertyUpdated ) {
    const auto e = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>( change );

    if( e->propertyName() == QByteArrayLiteral( "framesPerSecond" ) ) {
      const auto fps = e->value().toFloat();

      if( !qFuzzyCompare( fps, m_framesPerSecond ) ) {
        m_framesPerSecond = fps;
        const auto wasBlocked = blockNotifications( true );
        emit framesPerSecondChanged( fps );
        blockNotifications( wasBlocked );
      }

      return;
    }
  }

  QComponent::sceneChangeEvent( change );
}

Qt3DCore::QNodeCreatedChangeBasePtr FpsMonitor::createNodeCreationChange() const {
  auto creationChange = Qt3DCore::QNodeCreatedChangePtr<FpsMonitorData>::create( this );
  auto& data = creationChange->data;
  data.rollingMeanFrameCount = m_rollingMeanFrameCount;
  return creationChange;
}
