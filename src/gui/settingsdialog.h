#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QVector3D>

namespace Ui {
  class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

  public:
    explicit SettingsDialog( QWidget* parent = nullptr );
    ~SettingsDialog();

  signals:
    void antennaPositionChanged( QVector3D );
    void wheelbaseChanged( float );
    void hitchPositionChanged( QVector3D );
    void cameraChanged( int );

  public slots:
    void toggleVisibility();


  private slots:
    void on_dsb_antennaX_valueChanged( double arg1 );
    void on_dsb_antennaY_valueChanged( double arg1 );
    void on_dsb_antennaZ_valueChanged( double arg1 );

    void on_dsb_hitchLenght_valueChanged( double arg1 );

    void on_dsb_wheelbase_valueChanged( double arg1 );

    void on_rb_fixedCamera_clicked();
    void on_rb_firstPersonCamera_clicked();
    void on_rb_orbitCamera_clicked();

  private:
    void emitAntennaPosition();
    Ui::SettingsDialog* ui;
};

#endif // SETTINGSDIALOG_H
