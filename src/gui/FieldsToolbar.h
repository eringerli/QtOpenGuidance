// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#pragma once

#include <QGroupBox>
class QBoxLayout;

namespace Ui {
  class FieldsToolbar;
}

class FieldsToolbar : public QGroupBox {
  Q_OBJECT

public:
  explicit FieldsToolbar( QWidget* parent = nullptr );
  ~FieldsToolbar();

Q_SIGNALS:
  void continousRecordToggled( const bool );
  void recordPoint();

  void recordOnEdgeOfImplementChanged( const bool right );

private slots:
  void on_pbEdgeOfImplement_clicked( bool checked );

private:
  Ui::FieldsToolbar* ui        = nullptr;
  QBoxLayout*        boxLayout = nullptr;
};
