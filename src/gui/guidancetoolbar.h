#ifndef GUIDANCETOOLBAR_H
#define GUIDANCETOOLBAR_H

#include <QGroupBox>

namespace Ui {
class GuidanceToolbar;
}

class GuidanceToolbar : public QGroupBox
{
    Q_OBJECT

public:
    explicit GuidanceToolbar(QWidget *parent = nullptr);
    ~GuidanceToolbar();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_btn_settings_clicked();

signals:
    void simulatorChanged(bool);
    void toggleSettings();

private:
    Ui::GuidanceToolbar *ui;
};

#endif // GUIDANCETOOLBAR_H
