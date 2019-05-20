#include "simulatortoolbar.h"
#include "ui_simulatortoolbar.h"

SimulatorToolbar::SimulatorToolbar(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::SimulatorToolbar)
{
    ui->setupUi(this);
}

SimulatorToolbar::~SimulatorToolbar()
{
    delete ui;
}

void SimulatorToolbar::on_sl_velocity_valueChanged(int value)
{
    emit velocityChanged((float)value/10);
}

void SimulatorToolbar::on_sl_steerangle_valueChanged(int value)
{
    emit steerangleChanged((float)value/10);
}

void SimulatorToolbar::on_sl_steerangle_2_valueChanged(int value)
{
    emit frequencyChanged(value);
}
