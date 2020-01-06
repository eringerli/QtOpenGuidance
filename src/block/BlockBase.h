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

#ifndef BLOCKBASE_H
#define BLOCKBASE_H

#include <QObject>
#include <QString>
#include <QLatin1String>
#include <QStringLiteral>

#include <QtWidgets>
#include <QComboBox>

#include <QJsonObject>

#include <Qt3DCore/QEntity>

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

    virtual BlockBase* createNewObject() = 0;

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) = 0;

    QNEBlock* createBaseBlock( QGraphicsScene* scene, QObject* obj, bool systemBlock = false ) {
      auto* b = new QNEBlock( obj, systemBlock );

      scene->addItem( b );

      b->addPort( getNameOfFactory(), QLatin1String(), false, QNEPort::NamePort );
      b->addPort( getNameOfFactory(),  QLatin1String(), false, QNEPort::TypePort );

      return b;
    }

};

#endif // BLOCKBASE_H
