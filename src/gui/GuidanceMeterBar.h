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
    void setName( const QString& name );

    void setCaptionEnabled( bool enabled );
    void setFontOfLabel( QFont font );

  private:
    Ui::GuidanceMeterBar* ui;

  public:
    const QFont& fontOfLabel();
    bool captionEnabled();

    int precision = 0;
    int fieldWidth = 0;
    double scale = 1;
};

#endif // GUIDANCEMETERBAR_H
