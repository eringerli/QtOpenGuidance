#ifndef GUIDANCEXTEBAR_H
#define GUIDANCEXTEBAR_H

#include <QGroupBox>

namespace Ui {
  class GuidanceXteBar;
}

class GuidanceXteBar : public QGroupBox {
    Q_OBJECT

  public:
    explicit GuidanceXteBar( QWidget* parent = nullptr );
    ~GuidanceXteBar();

  public slots:
    void setXte( float xte );
    void setName( QString name );

  private:
    Ui::GuidanceXteBar* ui;
};

#endif // GUIDANCEXTEBAR_H
