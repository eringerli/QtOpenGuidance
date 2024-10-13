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

#include "gui/MyMainWindow.h"

Implement::Implement( const QString&                         uniqueName,
                      MyMainWindow*                          mainWindow,
                      KDDockWidgets::QtWidgets::DockWidget** firstDock,
                      const int                              idHint,
                      const bool                             systemBlock,
                      const QString                          type )
    : BlockBase( idHint, systemBlock, type ), firstDock( firstDock ) {
  widget = new ImplementToolbar( this, mainWindow );
  dock   = new KDDockWidgets::QtWidgets::DockWidget( uniqueName );

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
  BlockBase::emitConfigSignals();

  double width = 0;

  for( const auto& section : std::as_const( sections ) ) {
    width += section->widthOfSection - ( section->overlapLeft + section->overlapRight );
  }

  Q_EMIT leftEdgeChanged( Eigen::Vector3d( 0, float( -width / 2 ), 0 ), CalculationOption::Option::None );
  Q_EMIT rightEdgeChanged( Eigen::Vector3d( 0, float( width / 2 ), 0 ), CalculationOption::Option::None );
  Q_EMIT triggerLocalPose( Eigen::Vector3d( 0, 0, 0 ),
                           Eigen::Quaterniond( 0, 0, 0, 0 ),
                           CalculationOption::CalculateLocalOffsets | CalculationOption::NoOrientation );
  Q_EMIT implementChanged( this );
}

void
Implement::toJSON( QJsonObject& valuesObject ) const {
  if( sections.size() > 1 ) {
    QJsonArray array;

    for( size_t i = 1; i < sections.size(); ++i ) {
      QJsonObject sectionObject;
      sectionObject[QStringLiteral( "overlapLeft" )]    = sections[i]->overlapLeft;
      sectionObject[QStringLiteral( "widthOfSection" )] = sections[i]->widthOfSection;
      sectionObject[QStringLiteral( "overlapRight" )]   = sections[i]->overlapRight;
      array.append( sectionObject );
    }

    valuesObject[QStringLiteral( "Sections" )] = array;
  }
}

void
Implement::fromJSON( const QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "Sections" )].isArray() ) {
    QJsonArray sectionArray = valuesObject[QStringLiteral( "Sections" )].toArray();

    for( const auto& sectionIndex : std::as_const( sectionArray ) ) {
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
  BlockBase::setName( name );
  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "SC: " ) + name );
}

std::unique_ptr< BlockBase >
ImplementFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< Implement >( idHint, getNameOfFactory() + QString::number( idHint ), mainWindow, &firstDock );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( firstDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    firstDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, firstDock );
  }

  obj->addOutputPort(
    QStringLiteral( "Trigger Calculation of Local Pose" ), obj.get(), QLatin1StringView( SIGNAL( triggerLocalPose( POSE_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Implement Data" ), obj.get(), QLatin1StringView( SIGNAL( implementChanged( const QPointer< Implement > ) ) ) );
  obj->addOutputPort( QStringLiteral( "Section Control Data" ), obj.get(), QLatin1StringView( SIGNAL( sectionsChanged() ) ) );
  obj->addOutputPort(
    QStringLiteral( "Position Left Edge" ), obj.get(), QLatin1StringView( SIGNAL( leftEdgeChanged( VECTOR_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Position Right Edge" ), obj.get(), QLatin1StringView( SIGNAL( rightEdgeChanged( VECTOR_SIGNATURE ) ) ) );

  model->resetModel();

  return obj;
}
