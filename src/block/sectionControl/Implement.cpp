// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Implement.h"
#include "ImplementSection.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QPointer>

#include <QAction>
#include <QMenu>

#include "gui/model/ImplementBlockModel.h"
#include "gui/toolbar/ImplementToolbar.h"

#include "qneblock.h"
#include "qneport.h"

#include "gui/MyMainWindow.h"

Implement::Implement( const QString& uniqueName, MyMainWindow* mainWindow, KDDockWidgets::DockWidget** firstDock )
    : BlockBase(), firstDock( firstDock ) {
  widget = new ImplementToolbar( this, mainWindow );
  dock   = new KDDockWidgets::DockWidget( uniqueName );

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

void
Implement::emitConfigSignals() {
  double width = 0;

  for( const auto& section : qAsConst( sections ) ) {
    width += section->widthOfSection - ( section->overlapLeft + section->overlapRight );
  }

  Q_EMIT leftEdgeChanged( Eigen::Vector3d( 0, float( -width / 2 ), 0 ), CalculationOption::Option::None );
  Q_EMIT rightEdgeChanged( Eigen::Vector3d( 0, float( width / 2 ), 0 ), CalculationOption::Option::None );
  Q_EMIT triggerLocalPose( Eigen::Vector3d( 0, 0, 0 ),
                           Eigen::Quaterniond( 0, 0, 0, 0 ),
                           CalculationOption::CalculateLocalOffsets | CalculationOption::NoOrientation );
  Q_EMIT implementChanged( this );
}

QJsonObject
Implement::toJSON() {
  if( sections.size() > 1 ) {
    QJsonArray array;

    for( size_t i = 1; i < sections.size(); ++i ) {
      QJsonObject sectionObject;
      sectionObject[QStringLiteral( "overlapLeft" )]    = sections[i]->overlapLeft;
      sectionObject[QStringLiteral( "widthOfSection" )] = sections[i]->widthOfSection;
      sectionObject[QStringLiteral( "overlapRight" )]   = sections[i]->overlapRight;
      array.append( sectionObject );
    }

    QJsonObject valuesObject;
    valuesObject[QStringLiteral( "Sections" )] = array;
    return valuesObject;
  }

  return QJsonObject();
}

void
Implement::fromJSON( QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "Sections" )].isArray() ) {
    QJsonArray sectionArray = valuesObject[QStringLiteral( "Sections" )].toArray();

    for( const auto& sectionIndex : qAsConst( sectionArray ) ) {
      QJsonObject sectionObject = sectionIndex.toObject();
      sections.push_back( new ImplementSection( sectionObject[QStringLiteral( "overlapLeft" )].toDouble( 0 ),
                                                sectionObject[QStringLiteral( "widthOfSection" )].toDouble( 0 ),
                                                sectionObject[QStringLiteral( "overlapRight" )].toDouble( 0 ) ) );
    }
  }
}

void
Implement::emitImplementChanged() {
  Q_EMIT implementChanged( QPointer< Implement >( this ) );
}

void
Implement::emitSectionsChanged() {
  Q_EMIT sectionsChanged();
}

bool
Implement::isSectionOn( const size_t sectionNr ) {
  const auto& state0         = sections.at( 0 )->state();
  const bool  globalForceOff = state0.testFlag( ImplementSection::State::ForceOff );
  const bool  globalForceOn  = state0.testFlag( ImplementSection::State::ForceOn );

  const auto& section = sections.at( sectionNr + 1 );

  return !globalForceOff && ( !section->state().testFlag( ImplementSection::State::ForceOff ) ) &&
         ( globalForceOn || section->isSectionOn() );
}

void
Implement::setName( const QString& name ) {
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "SC: " ) + name );
}

QNEBlock*
ImplementFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* obj = new Implement( getNameOfFactory() + QString::number( id ), mainWindow, &firstDock );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( firstDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    firstDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, firstDock );
  }

  b->addOutputPort( QStringLiteral( "Trigger Calculation of Local Pose" ), QLatin1String( SIGNAL( triggerLocalPose( POSE_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Implement Data" ), QLatin1String( SIGNAL( implementChanged( const QPointer< Implement > ) ) ) );
  b->addOutputPort( QStringLiteral( "Section Control Data" ), QLatin1String( SIGNAL( sectionsChanged() ) ) );
  b->addOutputPort( QStringLiteral( "Position Left Edge" ), QLatin1String( SIGNAL( leftEdgeChanged( VECTOR_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Position Right Edge" ), QLatin1String( SIGNAL( rightEdgeChanged( VECTOR_SIGNATURE ) ) ) );

  model->resetModel();

  return b;
}
