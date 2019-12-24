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

#ifndef POSECACHE_H
#define POSECACHE_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"

class PoseSynchroniser : public BlockBase {
    Q_OBJECT

  public:
    explicit PoseSynchroniser()
      : BlockBase() {}

  public slots:
    void setPosition( Point_3 position ) {
      this->position = position;
      emit poseChanged( this->position, orientation, PoseOption::NoOptions );
    }

    void setOrientation( QQuaternion value ) {
      orientation = value;
    }

  signals:
    void poseChanged( Point_3, QQuaternion, PoseOption::Options );

  public:
    virtual void emitConfigSignals() override {
      emit poseChanged( position, orientation, PoseOption::NoOptions );
    }

  public:
    Point_3 position = Point_3();
    QQuaternion orientation = QQuaternion();
};

class PoseSynchroniserFactory : public BlockFactory {
    Q_OBJECT

  public:
    PoseSynchroniserFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Pose Synchroniser" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new PoseSynchroniser();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( "Position", SLOT( setPosition( Point_3 ) ) );
      b->addInputPort( "Orientation", SLOT( setOrientation( QQuaternion ) ) );

      b->addOutputPort( "Pose", SIGNAL( poseChanged( Point_3, QQuaternion, PoseOption::Options ) ) );

      return b;
    }
};

#endif // POSECACHE_H

