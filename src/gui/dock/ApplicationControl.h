#ifndef APPLICATIONCONTROL_H
  #define APPLICATIONCONTROL_H

#include "qgroupbox.h"
#include <QGroupBox>

namespace Ui {
  class ApplicationControl;
}

class ApplicationControl : public QGroupBox {
  Q_OBJECT

public:
  explicit ApplicationControl(QWidget *parent = nullptr);
  ~ApplicationControl();

signals:
  void requestFullscreen();
  void requestClose();

private slots:
  void on_pbFullscreen_clicked();
  void on_pbClose_clicked();

private:
  Ui::ApplicationControl *ui;
};

#endif // APPLICATIONCONTROL_H
