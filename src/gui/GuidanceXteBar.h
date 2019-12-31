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
    void setName( const QString& name );

  private:
    Ui::GuidanceXteBar* ui = nullptr;
};

#endif // GUIDANCEXTEBAR_H
