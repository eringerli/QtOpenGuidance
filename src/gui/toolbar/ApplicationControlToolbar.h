#ifndef ApplicationControlToolbar_H
#define ApplicationControlToolbar_H

#include "qgroupbox.h"
#include <QGroupBox>

namespace Ui {
  class ApplicationControlToolbar;
}

class ApplicationControlToolbar : public QGroupBox {
  Q_OBJECT

public:
  explicit ApplicationControlToolbar( QWidget* parent = nullptr );
  ~ApplicationControlToolbar();

signals:
  void requestFullscreen();
  void requestClose();
  void requestSaveConfig();

private slots:
  void on_pbFullscreen_clicked();
  void on_pbClose_clicked();

  void on_pbSaveConfig_clicked();

private:
  Ui::ApplicationControlToolbar* ui;
};

#endif // ApplicationControlToolbar_H
