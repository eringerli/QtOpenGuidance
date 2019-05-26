#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QVector3D>
#include <Qt3DCore/QEntity>

#include <QGraphicsScene>

#include "../PoseCache.h"

namespace Ui {
  class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

  public:
    explicit SettingsDialog( Qt3DCore::QEntity* rootEntity, QWidget* parent = nullptr );
    ~SettingsDialog();

    QGraphicsScene* getSceneOfConfigGraphicsView();

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

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

  private:
    void emitAntennaPosition();
    Ui::SettingsDialog* ui;

    Qt3DCore::QEntity* rootEntity;

    GuidanceFactory* poseCacheFactory;
    GuidanceFactory* tractorModelFactory;
    GuidanceFactory* trailerModelFactory;
    GuidanceFactory* fixedKinematicFactory;
    GuidanceFactory* trailerKinematicFactory;
    GuidanceFactory* vectorFactory;
    GuidanceFactory* lengthFactory;
};

#endif // SETTINGSDIALOG_H
