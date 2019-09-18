#ifndef GUIDANCEMETERBAR_H
#define GUIDANCEMETERBAR_H

#include <QGroupBox>

namespace Ui {
  class GuidanceMeterBar;
}

class GuidanceMeterBar : public QGroupBox {
    Q_OBJECT

  public:
    explicit GuidanceMeterBar( QWidget* parent = nullptr );
    ~GuidanceMeterBar();

  public slots:
    void setMeter( float meter );
    void setPrecision( float precision );
    void setScale( float scale );
    void setFieldWitdh( float fieldwitdh );
    void setName( QString name );

  private:
    Ui::GuidanceMeterBar* ui;

    int precision = -1;
    int fieldwitdh = 0;
    float scale = 1;
};

#endif // GUIDANCEMETERBAR_H
