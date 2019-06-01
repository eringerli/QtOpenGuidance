#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QObject>

#include <QGroupBox>

namespace Ui {
  class CameraToolbar;
}

class CameraToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit CameraToolbar( QWidget* parent = nullptr );
    ~CameraToolbar();

  private slots:
    void on_pbUp_clicked();

    void on_pbDown_clicked();

    void on_pbZoomIn_clicked();

    void on_pbZoomOut_clicked();

    void on_pbPanLeft_clicked();

    void on_pbPanRight_clicked();

    void on_pbReset_clicked();

    void on_comboBox_currentIndexChanged( int index );

  signals:
    void zoomIn();
    void zoomOut();
    void tiltUp();
    void tiltDown();
    void panRight();
    void panLeft();
    void resetCamera();
    void setMode( int );

  private:
    Ui::CameraToolbar* ui;
};

#endif // CAMERACONTROL_H
