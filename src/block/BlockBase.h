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

//#include <QJsonObject>

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

class BlockFactory;

class BlockFactory : public QObject {
    Q_OBJECT

  public:
    BlockFactory() {}
    ~BlockFactory() {}

    virtual QString getNameOfFactory() = 0;

    virtual QString getPrettyNameOfFactory() {
      return getNameOfFactory();
    };

    virtual QString getCategoryOfFactory() = 0;

//    virtual QString getDescriptionOfFactory() = 0;

    void addToCombobox( QComboBox* combobox );

    void addToTreeWidget( QTreeWidget* treeWidget );

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) = 0;

    QNEBlock* createBaseBlock( QGraphicsScene* scene, QObject* obj, int id, bool systemBlock = false );

    bool isIdUnique( QGraphicsScene* scene, int id );

  protected:
    const QColor modelColor = QColor( QStringLiteral( "moccasin" ) );
    const QColor dockColor = QColor( QStringLiteral( "DarkSalmon" ) );
    const QColor inputDockColor = QColor( QStringLiteral( "lightsalmon" ) );
    const QColor parserColor = QColor( QStringLiteral( "mediumaquamarine" ) );
    const QColor valueColor = QColor( QStringLiteral( "gold" ) );
    const QColor inputOutputColor = QColor( QStringLiteral( "cornflowerblue" ) );
    const QColor converterColor = QColor( QStringLiteral( "lightblue" ) );
    const QColor arithmeticColor = QColor( QStringLiteral( "DarkKhaki" ) );
};
