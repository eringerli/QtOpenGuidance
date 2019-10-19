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

#include "../gui/ImplementBlockModel.h"

#ifndef IMPLEMENT_H
#define IMPLEMENT_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>
#include "../kinematic/Tile.h"
#include "../kinematic/PoseOptions.h"

#include "BlockBase.h"

class ImplementSection {
  public:
    ImplementSection() {}
    ImplementSection( double overlapLeft, double widthOfSection, double overlapRight )
      : overlapLeft( overlapLeft ), widthOfSection( widthOfSection ), overlapRight( overlapRight ) {}

  public:
    double overlapLeft = 0;
    double widthOfSection = 0;
    double overlapRight = 0;
};

class Implement : public BlockBase {
    Q_OBJECT

  public:
    explicit Implement( Tile* tile )
      : BlockBase() {
      this->tile00 = tile->getTileForOffset( 0, 0 );
    }

    void emitConfigSignals() override {
      double width = 0;

      foreach( const QSharedPointer<ImplementSection> section, sections ) {
        width +=  section->widthOfSection - section->overlapLeft - section->overlapRight;
      }

      emit leftEdgeChanged( QVector3D( 0, float( -width / 2 ), 0 ) );
      emit rightEdgeChanged( QVector3D( 0, float( width / 2 ), 0 ) );
      emit sectionsChanged( sections );
      emit triggerLocalPose( tile00, QVector3D(), QQuaternion(),
                             PoseOption::CalculateLocalOffsets |
                             PoseOption::CalculateWithoutTiling |
                             PoseOption::CalculateWithoutOrientation |
                             PoseOption::CalculateFromPivotPoint );
    }

    void toJSON( QJsonObject& json ) override {
      QJsonArray array;

      foreach( const QSharedPointer<ImplementSection> section, sections ) {
        QJsonObject sectionObject;
        sectionObject["overlapLeft"] = section->overlapLeft;
        sectionObject["widthOfSection"] = section->widthOfSection;
        sectionObject["overlapRight"] = section->overlapRight;
        array.append( sectionObject );
      }

      QJsonObject valuesObject;
      valuesObject["Sections"] = array;
      json["values"] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json["values"].isObject() ) {
        QJsonObject valuesObject = json["values"].toObject();

        if( valuesObject["Sections"].isArray() ) {
          QJsonArray sectionArray = valuesObject["Sections"].toArray();

          for( auto&& sectionIndex : sectionArray ) {
            QJsonObject sectionObject = sectionIndex.toObject();
            sections.append(
                    QSharedPointer<ImplementSection>(
                            new ImplementSection( sectionObject["overlapLeft"].toDouble( 0 ),
                                                  sectionObject["widthOfSection"].toDouble( 0 ),
                                                  sectionObject["overlapRight"].toDouble( 0 ) ) ) );
          }
        }
      }
    }

  signals:
    void triggerLocalPose( Tile*, QVector3D, QQuaternion, PoseOption::Options );
    void sectionsChanged( QVector<QSharedPointer<ImplementSection>> );
    void leftEdgeChanged( QVector3D );
    void rightEdgeChanged( QVector3D );

  public:
    QVector<QSharedPointer<ImplementSection>> sections;
    Tile* tile00 = nullptr;
};

class ImplementFactory : public BlockFactory {
    Q_OBJECT

  public:
    ImplementFactory( Tile* tile, ImplementBlockModel* model )
      : BlockFactory(),
        tile( tile ),
        model( model ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Implement" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new Implement( tile );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addOutputPort( "Trigger Calculation of Local Pose", SIGNAL( triggerLocalPose( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addOutputPort( "Section Controll Data", SIGNAL( sectionsChanged( QVector<QSharedPointer<ImplementSection>> ) ) );
      b->addOutputPort( "Position Left Edge", SIGNAL( leftEdgeChanged( QVector3D ) ) );
      b->addOutputPort( "Position Right Edge", SIGNAL( rightEdgeChanged( QVector3D ) ) );

      model->resetModel();

      return b;
    }

  private:
    Tile* tile = nullptr;
    ImplementBlockModel* model = nullptr;
};

#endif // IMPLEMENT_H
