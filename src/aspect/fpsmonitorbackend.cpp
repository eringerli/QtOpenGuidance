// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#include "fpsmonitorbackend.h"
#include "fpsmonitor.h"
#include "fpsaspect.h"

#include <Qt3DCore/QNodeCreatedChange>
#include <Qt3DCore/QPropertyUpdatedChange>

FpsMonitorBackend::FpsMonitorBackend()
  : Qt3DCore::QBackendNode( Qt3DCore::QBackendNode::ReadWrite ) {
}

void FpsMonitorBackend::addFpsSample( float fpsSample ) {
  // Update the moving average
  m_average.addSample( fpsSample );

  // Calculate the average and notify the frontend
  const auto meanFps = m_average.average();
  auto e = Qt3DCore::QPropertyUpdatedChangePtr::create( peerId() );
  e->setDeliveryFlags( Qt3DCore::QSceneChange::Nodes );
  e->setPropertyName( "framesPerSecond" );
  e->setValue( QVariant::fromValue( meanFps ) );
  notifyObservers( e );
}

void FpsMonitorBackend::initializeFromPeer( const Qt3DCore::QNodeCreatedChangeBasePtr& change ) {
  const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<FpsMonitorData>>( change );
  const auto& data = typedChange->data;
  m_rollingMeanFrameCount = data.rollingMeanFrameCount;
  m_average = MovingAverage( m_rollingMeanFrameCount );
}

void FpsMonitorBackend::sceneChangeEvent( const Qt3DCore::QSceneChangePtr& e ) {
  if( e->type() == Qt3DCore::PropertyUpdated ) {
    const auto change = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>( e );
    qDebug() << change->propertyName();

    if( change->propertyName() == QByteArrayLiteral( "rollingMeanFrameCount" ) ) {
      const auto newValue = change->value().toInt();

      if( newValue != m_rollingMeanFrameCount ) {
        m_rollingMeanFrameCount = newValue;
        m_average = MovingAverage( newValue );
      }

      return;
    }
  }

  QBackendNode::sceneChangeEvent( e );
}

FpsMonitorMapper::FpsMonitorMapper( FpsAspect* aspect )
  : m_aspect( aspect ) {
  Q_ASSERT( m_aspect );
}

Qt3DCore::QBackendNode* FpsMonitorMapper::create( const Qt3DCore::QNodeCreatedChangeBasePtr& change ) const {
  auto fpsMonitor = new FpsMonitorBackend;
  m_aspect->addFpsMonitor( change->subjectId(), fpsMonitor );
  return fpsMonitor;
}

Qt3DCore::QBackendNode* FpsMonitorMapper::get( Qt3DCore::QNodeId id ) const {
  return m_aspect->fpsMonitor( id );
}

void FpsMonitorMapper::destroy( Qt3DCore::QNodeId id ) const {
  auto fpsMonitor = m_aspect->takeFpsMonitor( id );
  delete fpsMonitor;
}
