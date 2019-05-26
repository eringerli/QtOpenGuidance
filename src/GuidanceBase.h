#ifndef GUIDANCEBASE_H
#define GUIDANCEBASE_H

#include <QtCore/QObject>

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
    ~GuidanceBase() {}

    virtual void emitConfigSignals() {}

//    virtual QJsonObject toJSON() = 0;
//    virtual void fromJSON(QJsonObject& json) = 0;

  public:
};

class GuidanceFactory : public QObject {
    Q_OBJECT

  public:
    GuidanceFactory() {}
    ~GuidanceFactory() {}

    virtual void addToCombobox( QComboBox* combobox ) = 0;

    virtual GuidanceBase* createNewObject() = 0;

    virtual void createBlock( QGraphicsScene* scene, QObject* obj ) = 0;

  public:
};

#endif // GUIDANCEBASE_H
