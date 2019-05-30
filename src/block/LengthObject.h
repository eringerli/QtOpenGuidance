// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef LENGTHWIDGET_H
#define LENGTHWIDGET_H

#include <QWidget>

#include "GuidanceBase.h"

class LengthObject : public GuidanceBase {
    Q_OBJECT

  public:
    explicit LengthObject()
      : length( 0 ) {
      m_id = getNextUserId();
    }

  signals:
    void lengthChanged( float );

  public:
    float length;
};

class LengthFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    LengthFactory()
      : GuidanceFactory() {}
    ~LengthFactory() {}

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( QStringLiteral( "Length" ), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new LengthObject();
    }

    virtual void createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( "Length", "", 0, QNEPort::NamePort );
      b->addPort( "Length", "", 0, QNEPort::TypePort );

      b->addOutputPort( "Length", SIGNAL( lengthChanged( float ) ) );
    }
};

#endif // LENGTHWIDGET_H
