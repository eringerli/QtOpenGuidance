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

#include <QObject>
#include <QString>
#include <QLatin1String>
#include <QStringLiteral>

#include <QtWidgets>
#include <QComboBox>

#include <QJsonObject>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QComponent>

#include "qneblock.h"
#include "qneport.h"

class BlockBase : public QObject {
    Q_OBJECT

  public:
    BlockBase() {}

    virtual void emitConfigSignals() {}

    virtual void toJSON( QJsonObject& ) {}
    virtual void fromJSON( QJsonObject& ) {}

    virtual void setName( const QString& ) {}
};

class BlockFactory : public QObject {
    Q_OBJECT

  public:
    BlockFactory() {}
    ~BlockFactory() {}

    virtual QString getNameOfFactory() = 0;

    virtual void addToCombobox( QComboBox* combobox ) = 0;

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) = 0;

    QNEBlock* createBaseBlock( QGraphicsScene* scene, QObject* obj,
                               int id, bool systemBlock = false,
                               QNEBlock::Flags flags = QNEBlock::Normal ) {
      if( id != 0 && !isIdUnique( scene, id ) ) {
        id = 0;
        qDebug() << "BlockFactory::createBaseBlock: ID conflict";
      }

      auto* b = new QNEBlock( obj, id, systemBlock, flags );

      scene->addItem( b );

      b->addPort( getNameOfFactory(), QLatin1String(), false, QNEPort::NamePort );
      b->addPort( getNameOfFactory(),  QLatin1String(), false, QNEPort::TypePort );

      return b;
    }

    bool isIdUnique( QGraphicsScene* scene, int id ) {
      const auto& constRefOfList = scene->items();

      for( const auto& item : constRefOfList ) {
        auto* block = qgraphicsitem_cast<QNEBlock*>( item );

        if( block != nullptr ) {
          if( block->id == id ) {
            return false;
          }
        }
      }

      return true;
    }
};

class EmbeddedBlockDummy {
};

Q_DECLARE_METATYPE( EmbeddedBlockDummy )
