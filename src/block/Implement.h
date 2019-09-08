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
    explicit Implement()
      : BlockBase() {}

    void emitConfigSignals() override {
      emit sectionsChanged( sections );
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

          for( int sectionIndex = 0; sectionIndex < sectionArray.size(); ++sectionIndex ) {
            QJsonObject sectionObject = sectionArray[sectionIndex].toObject();
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
    void sectionsChanged( QVector<QSharedPointer<ImplementSection>> );

  public:
    QVector<QSharedPointer<ImplementSection>> sections;
};

class ImplementFactory : public BlockFactory {
    Q_OBJECT

  public:
    ImplementFactory( ImplementBlockModel* model )
      : BlockFactory(),
        model( model ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Implement" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new Implement();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addOutputPort( "String", SIGNAL( stringChanged( QString ) ) );

      model->resetModel();

      return b;
    }

  private:
    ImplementBlockModel* model = nullptr;
};

#endif // IMPLEMENT_H
