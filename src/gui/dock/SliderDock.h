// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QGroupBox>

namespace Ui {
  class SliderDock;
}

class SliderDock : public QGroupBox {
  Q_OBJECT

public:
  explicit SliderDock( QWidget* parent = nullptr );
  ~SliderDock();

  void setValue( const double );
  void setDecimals( const int decimals );
  void setMaximum( const double );
  void setMinimum( const double );
  void setDefaultValue( const double );
  void setUnit( const QString& unit );
  void setSliderInverted( const bool inverted );

  double        getValue() const;
  int           getDecimals() const;
  double        getMaximum() const;
  double        getMinimum() const;
  double        getDefaultValue() const;
  const QString getUnit() const;
  bool          getSliderInverted() const;

private Q_SLOTS:
  void on_slValue_valueChanged( int value );
  void on_dsbValue_valueChanged( double value );
  void on_pbValueReset_clicked();

Q_SIGNALS:
  void valueChanged( const double );

private:
  Ui::SliderDock* ui = nullptr;

  double defaultValue = 0;

public:
  bool resetOnStart = false;
};
