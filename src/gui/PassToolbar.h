#pragma once

#include <QGroupBox>

namespace Ui {
  class PassToolbar;
}

class PassToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit PassToolbar( QWidget* parent = nullptr );
    ~PassToolbar();

    void emitPassSettings();

  signals:
    void passSettingsChanged( int, int, bool, bool );
    void minimalSkipChanged( int );
    void passNumberChanged( int );

  private slots:
    void on_sbForwardPasses_valueChanged( int arg1 );

    void on_sbReversePasses_valueChanged( int arg1 );

    void on_rbStartRight_toggled( bool checked );

    void on_pbResetForwardPasses_clicked();

    void on_pbResetReversePasses_clicked();

    void on_pbSetReverseToForwardPasses_clicked();

    void on_pbResetPassNumber_clicked();

    void on_pbSetPassNumber_clicked();

    void on_rbStartLeft_toggled( bool checked );

    void on_cbMirror_stateChanged( int arg1 );

  private:
    Ui::PassToolbar* ui = nullptr;
};

