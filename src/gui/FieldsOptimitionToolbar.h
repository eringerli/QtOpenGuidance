#ifndef FIELDSOPTIMITION_H
#define FIELDSOPTIMITION_H

#include <QGroupBox>

namespace Ui {
  class FieldsOptimitionToolbar;
}

class FieldsOptimitionToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit FieldsOptimitionToolbar( QWidget* parent = nullptr );
    ~FieldsOptimitionToolbar();

    enum class AlphaType : uint_fast8_t {
      None = 0,
      Optimal,
      Solid,
      Custom
    };

  signals:
    void recalculateField();
    void recalculateFieldSettingsChanged( FieldsOptimitionToolbar::AlphaType alphaType, double customAlpha, double maxDeviation, double distanceBetweenConnectPoints );

  public slots:
    void setAlpha( double optimal, double solid );

  private slots:
    void on_pbRecalculate_clicked();

    void on_cbAlphaShape_currentTextChanged( const QString& );

    void on_cbConnectEndToStart_stateChanged( int );

    void on_dsbDistanceConnectingPoints_valueChanged( double );

    void on_dsbAlpha_valueChanged( double arg1 );

    void on_dsbMaxDeviation_valueChanged( double arg1 );

  private:
    Ui::FieldsOptimitionToolbar* ui = nullptr;

    double optimalAlpha = 20;
    double solidAlpha = 100;
};

#endif // FIELDSOPTIMITION_H
