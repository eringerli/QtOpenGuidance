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

#ifndef METERBARMODEL_H
#define METERBARMODEL_H

#include <QObject>

#include "../gui/GuidanceMeterBar.h"

#include "BlockBase.h"

class MeterBarModel : public BlockBase {
    Q_OBJECT

  public:
    explicit MeterBarModel( QWidget* parent, QHBoxLayout* hLayout )
      : BlockBase() {
      guidanceMeterBar = new GuidanceMeterBar( parent );
      hLayout->addWidget( guidanceMeterBar );
    }

    ~MeterBarModel() {
      guidanceMeterBar->deleteLater();
    }


  public slots:
    void setName( QString name ) {
      if( name == QStringLiteral( "MeterBarModel" ) ) {
        guidanceMeterBar->setName( QString() );
      } else {
        guidanceMeterBar->setName( name );
      }
    }

    void setMeter( float meter ) {
      guidanceMeterBar->setMeter( meter );
    }

    void setPrecision( float precision ) {
      guidanceMeterBar->setPrecision( precision );
    }

    void setScale( float scale ) {
      guidanceMeterBar->setScale( scale );
    }

    void setFieldWitdh( float fieldWitdh ) {
      guidanceMeterBar->setFieldWitdh( fieldWitdh );
    }

  public:
    GuidanceMeterBar* guidanceMeterBar = nullptr;
};

class MeterBarModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    MeterBarModelFactory( QWidget* parent, QHBoxLayout* hlayout )
      : BlockFactory(),
        parent( parent ), hlayout( hlayout ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "MeterBarModel" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new MeterBarModel( parent, hlayout );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( "Number", SLOT( setMeter( float ) ) );
      b->addInputPort( "Precision", SLOT( setPrecision( float ) ) );
      b->addInputPort( "Scale", SLOT( setScale( float ) ) );
      b->addInputPort( "FieldWitdh", SLOT( setFieldWitdh( float ) ) );

      return b;
    }

  private:
    QWidget* parent = nullptr;
    QHBoxLayout* hlayout = nullptr;
};

#endif // METERBARMODEL_H
