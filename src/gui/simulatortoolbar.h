#ifndef SIMULATORTOOLBAR_H
#define SIMULATORTOOLBAR_H

#include <QGroupBox>

namespace Ui {
  class SimulatorToolbar;
}

class SimulatorToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit SimulatorToolbar( QWidget* parent = nullptr );
    ~SimulatorToolbar();

  private slots:
    void on_sl_velocity_valueChanged( int value );
    void on_sl_steerangle_valueChanged( int value );

    void on_sl_steerangle_2_valueChanged( int value );

  signals:
    void velocityChanged( float );
    void steerangleChanged( float );
    void frequencyChanged( int );

  private:
    Ui::SimulatorToolbar* ui;
};

#endif // SIMULATORTOOLBAR_H
