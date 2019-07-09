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

#ifndef GUIDANCEBASE_H
#define GUIDANCEBASE_H

#include <QObject>

#include <QtWidgets>
#include <QComboBox>

#include <QJsonObject>

#include <Qt3DCore/QEntity>

#include "qneblock.h"
#include "qneport.h"

class GuidanceBase : public QObject {
    Q_OBJECT

  public:
    GuidanceBase() {}

    virtual void emitConfigSignals() {}

    virtual void toJSON( QJsonObject& /*json*/ ) {}
    virtual void fromJSON( QJsonObject& /*json*/ ) {}

    virtual void setName( QString ) {}
};

class GuidanceFactory : public QObject {
    Q_OBJECT

  public:
    GuidanceFactory() {}
    ~GuidanceFactory() {}

    virtual QString getNameOfFactory() = 0;

    virtual void addToCombobox( QComboBox* combobox ) = 0;

    virtual GuidanceBase* createNewObject() = 0;

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) = 0;

};

#endif // GUIDANCEBASE_H
