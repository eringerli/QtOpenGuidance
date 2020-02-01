#ifndef VALUEDOCK_H
#define VALUEDOCK_H

#include <QGroupBox>

namespace Ui {
  class ValueDock;
}

class ValueDock : public QGroupBox {
    Q_OBJECT

  public:
    explicit ValueDock( QWidget* parent = nullptr );
    ~ValueDock();

  public slots:
    void setMeter( float arg );
    void setName( const QString& name );

    void setCaptionEnabled( bool enabled );
    void setFontOfLabel( const QFont& font );

  private:
    Ui::ValueDock* ui = nullptr;

  public:
    const QFont& fontOfLabel();
    bool captionEnabled();

    int precision = 0;
    int fieldWidth = 0;
    double scale = 1;
};

#endif // VALUEDOCK_H
