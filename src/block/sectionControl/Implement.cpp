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

#include "Implement.h"
#include "ImplementSection.h"

#include <QPointer>
#include <QJsonArray>
#include <QJsonObject>

#include <QAction>
#include <QMenu>

#include "gui/model/ImplementBlockModel.h"
#include "gui/SectionControlToolbar.h"

#include "qneblock.h"
#include "qneport.h"

#include "gui/MyMainWindow.h"

Implement::Implement( const QString& uniqueName, MyMainWindow* mainWindow, KDDockWidgets::DockWidget** firstDock )
  : BlockBase(),
    firstDock( firstDock ) {
  widget = new SectionControlToolbar( this, mainWindow );
  dock = new KDDockWidgets::DockWidget( uniqueName );

  // add section 0: the section to control them all
  sections.push_back( new ImplementSection( 0, 0, 0 ) );
}

Implement::~Implement() {
  if( *firstDock == dock ) {
    *firstDock = nullptr;
  }

  widget->deleteLater();
  dock->deleteLater();
}

void Implement::emitConfigSignals() {
  double width = 0;

  for( const auto& section : qAsConst( sections ) ) {
    width +=  section->widthOfSection - section->overlapLeft - section->overlapRight;
  }

  Q_EMIT leftEdgeChanged( Eigen::Vector3d( 0, float( -width / 2 ), 0 ) );
  Q_EMIT rightEdgeChanged( Eigen::Vector3d( 0, float( width / 2 ), 0 ) );
  Q_EMIT triggerLocalPose( Eigen::Vector3d( 0, 0, 0 ),
                           Eigen::Quaterniond(),
                           PoseOption::CalculateLocalOffsets |
                           PoseOption::CalculateWithoutOrientation );
  Q_EMIT implementChanged( this );
}

void Implement::toJSON( QJsonObject& json ) {
  if( sections.size() > 1 ) {
    QJsonArray array;

    for( size_t i = 1; i < sections.size(); ++i ) {
      QJsonObject sectionObject;
      sectionObject[QStringLiteral( "overlapLeft" )] = sections[i]->overlapLeft;
      sectionObject[QStringLiteral( "widthOfSection" )] = sections[i]->widthOfSection;
      sectionObject[QStringLiteral( "overlapRight" )] = sections[i]->overlapRight;
      array.append( sectionObject );
    }

    QJsonObject valuesObject;
    valuesObject[QStringLiteral( "Sections" )] = array;
    json[QStringLiteral( "values" )] = valuesObject;
  }
}

void Implement::fromJSON( QJsonObject& json ) {
  if( json[QStringLiteral( "values" )].isObject() ) {
    QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

    if( valuesObject[QStringLiteral( "Sections" )].isArray() ) {
      QJsonArray sectionArray = valuesObject[QStringLiteral( "Sections" )].toArray();

      for( const auto& sectionIndex : qAsConst( sectionArray ) ) {
        QJsonObject sectionObject = sectionIndex.toObject();
        sections.push_back(
                new ImplementSection( sectionObject[QStringLiteral( "overlapLeft" )].toDouble( 0 ),
                                      sectionObject[QStringLiteral( "widthOfSection" )].toDouble( 0 ),
                                      sectionObject[QStringLiteral( "overlapRight" )].toDouble( 0 ) ) );
      }
    }
  }
}

void Implement::emitImplementChanged() {
  Q_EMIT implementChanged( QPointer<Implement>( this ) );

}

void Implement::emitSectionsChanged() {
  Q_EMIT sectionsChanged();
}

void Implement::setName( const QString& name ) {
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "SC: " ) + name );
}

QNEBlock* ImplementFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* object = new Implement( getNameOfFactory() + QString::number( id ),
                                mainWindow,
                                &firstDock );
  auto* b = createBaseBlock( scene, object, id );

  object->dock->setTitle( getNameOfFactory() );
  object->dock->setWidget( object->widget );

  menu->addAction( object->dock->toggleAction() );

  if( firstDock == nullptr ) {
    mainWindow->addDockWidget( object->dock, location );
    firstDock = object->dock;
  } else {
    mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnBottom, firstDock );
  }

  b->addOutputPort( QStringLiteral( "Trigger Calculation of Local Pose" ), QLatin1String( SIGNAL( triggerLocalPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
  b->addOutputPort( QStringLiteral( "Implement Data" ), QLatin1String( SIGNAL( implementChanged( const QPointer<Implement> ) ) ) );
  b->addOutputPort( QStringLiteral( "Section Control Data" ), QLatin1String( SIGNAL( sectionsChanged() ) ) );
  b->addOutputPort( QStringLiteral( "Position Left Edge" ), QLatin1String( SIGNAL( leftEdgeChanged( const Eigen::Vector3d& ) ) ) );
  b->addOutputPort( QStringLiteral( "Position Right Edge" ), QLatin1String( SIGNAL( rightEdgeChanged( const Eigen::Vector3d& ) ) ) );

  model->resetModel();

  return b;
}
