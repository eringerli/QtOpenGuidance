#ifndef GUIDANCETOOLBARTOP_H
#define GUIDANCETOOLBARTOP_H

#include <QGroupBox>

namespace Ui {
  class GuidanceToolbarTop;
}

class GuidanceToolbarTop : public QGroupBox {
    Q_OBJECT

  public:
    explicit GuidanceToolbarTop( QWidget* parent = nullptr );
    ~GuidanceToolbarTop();

  private slots:
    void on_pbLeft_clicked();
    void on_pbRight_clicked();

    void setXte( double xte );

  signals:
    void turnLeft();
    void turnRight();

  private:
    Ui::GuidanceToolbarTop* ui;
};

#endif // GUIDANCETOOLBARTOP_H
