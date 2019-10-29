// From here: https://www.kdab.com/writing-custom-qt-3d-aspect/

#ifndef UPDATEFPSMONITORSJOB_H
#define UPDATEFPSMONITORSJOB_H

#include <Qt3DCore/QAspectJob>
#include <QSharedPointer>

class FpsAspect;

class UpdateFpsMonitorsJob : public Qt3DCore::QAspectJob {
  public:
    explicit UpdateFpsMonitorsJob( FpsAspect* aspect );

    void setFrameTimeDelta( qint64 dt );
    void run() override;

  private:
    FpsAspect* m_aspect;
    qint64 m_dt;
};

using UpdateFpsMonitorsJobPtr = QSharedPointer<UpdateFpsMonitorsJob>;

#endif // UPDATEFPSMONITORSJOB_H
