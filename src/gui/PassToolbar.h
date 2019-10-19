#ifndef PASSTOOLBAR_H
#define PASSTOOLBAR_H

#include <QGroupBox>

namespace Ui {
  class PassToolbar;
}

class PassToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit PassToolbar( QWidget* parent = nullptr );
    ~PassToolbar();

  private:
    Ui::PassToolbar* ui;
};

#endif // PASSTOOLBAR_H
