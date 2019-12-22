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

    enum class AlphaType : uint8_t {
      None = 0,
      Optimal,
      Solid,
      Custom
    };

  signals:
    void recalculate( FieldsOptimitionToolbar::AlphaType alphaType, double alpha, double maxDeviation );

  public slots:
    void setAlpha( double optimal, double solid );


  private slots:
    void on_pbRecalculate_clicked();

    void on_cbAlphaShape_currentTextChanged( const QString& arg1 );

  private:
    Ui::FieldsOptimitionToolbar* ui;

    double optimalAlpha = 20;
    double solidAlpha = 100;
};

#endif // FIELDSOPTIMITION_H
