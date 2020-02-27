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

#include "GlobalPlannerLines.h"
#include <QScopedPointer>

#include <QFileDialog>

#include "../cgal.h"
#include "../kinematic/CgalWorker.h"

bool GlobalPlannerLines::isLineAlreadyInPlan( const std::shared_ptr<PathPrimitiveLine>& line ) {
  bool twinFound = false;

  for( const auto& step : * ( plan.plan ) ) {
    if( const auto* pathLine = step->castToLine() ) {
      if( ( *pathLine ) == ( *line ) ) {
        twinFound = true;
        break;
      }
    }
  }

  return twinFound;
}

void GlobalPlannerLines::sortPlan() {
  sort( plan.plan->begin( ), plan.plan->end( ), [ ]( const auto & lhs, const auto & rhs ) {
    const auto* lhsLine = lhs->castToLine();
    const auto* rhsLine = rhs->castToLine();

    return lhsLine->line.has_on_positive_side( rhsLine->line.point( 0 ) );
  } );
}

void GlobalPlannerLines::clearPlan() {
  plan.plan->clear();
}

void GlobalPlannerLines::showPlan() {
  if( !plan.plan->empty() ) {
    const Point_2 position2D = to2D( position );

    constexpr double range = 25;
    Iso_rectangle_2 viewBox( Bbox_2( position2D.x() - range, position2D.y() - range, position2D.x() + range, position2D.y() + range ) );

    QVector<QVector3D> positions;

    for( const auto& step : * ( plan.plan ) ) {
      if( const auto* pathLine = step->castToLine() ) {
        const auto& line = pathLine->line;

        CGAL::cpp11::result_of<Intersect_2( Segment_2, Line_2 )>::type
        result = intersection( viewBox, line );

        if( result ) {
          if( const Segment_2* segment = boost::get<Segment_2>( &*result ) ) {
            positions << QVector3D( segment->source().x(), segment->source().y(), 0 );
            positions << QVector3D( segment->target().x(), segment->target().y(), 0 );
          }
        }
      }
    }

    m_segmentsMesh->bufferUpdate( positions );
    m_segmentsEntity->setEnabled( true );
  }
}

void GlobalPlannerLines::createPlanAB() {
  if( abLine.squared_length() > 1 ) {
    Point_2 position2D = to2D( position );

    if( plan.plan->size() >= std::vector<std::shared_ptr<PathPrimitive>>::size_type( pathsInReserve * 2 + 1 ) ) {
      if( ( *( plan.plan->cbegin() + ( pathsInReserve - 1 ) ) )->castToLine()->line.has_on_positive_side( position2D ) &&
          ( *( plan.plan->cend() - pathsInReserve ) )->castToLine()->line.has_on_negative_side( position2D ) ) {
        return;
      }
    }

    qDebug() << "Long way...";

    Segment_2 ab2dSegment = to2D( abLine );
    Line_2 ab2D = ab2dSegment.supporting_line();
    Line_3 ab3D = Segment_3( Point_3( abLine.source().x(), abLine.source().y(), 0 ), Point_3( abLine.target().x(), abLine.target().y(), 0 ) ).supporting_line();
    Point_2 positionProjectedToAbLine = ab2D.projection( position2D );
    Segment_2 abPerpendicularSegment( position2D, positionProjectedToAbLine );

    Vector_3 north( Point_3( 0, 0, 0 ), Point_3( 1, 0, 0 ) );
    Vector_2 north2D( Point_2( 0, 0 ), Point_2( 1, 0 ) );

    //std::cout << "lines: ab2D:"<<ab2D<<", position2D:"<<position2D<<", abPerpendicular:"<<abPerpendicular<<std::endl;
    //std::cout << "angles: ab2D:"<<CGAL::approximate_angle(north,ab3D.to_vector())<<", position2D:"<<position2D<<", abPerpendicular:"<<abPerpendicular<<std::endl;

    double angleAB = CGAL::approximate_angle( north, ab3D.to_vector() );
//    std::cout << "CGAL::orientation(): " << CGAL::orientation( north2D, ab2D.to_vector() ) << ", has_on_positive_side:" << ab2D.has_on_positive_side( position2D ) << std::endl;
    double angleAbRad = normalizeAngleRadians( ( M_PI - qDegreesToRadians( angleAB ) ) * -CGAL::orientation( north2D, ab2D.to_vector() ) );

    double implementWidth = sqrt( implementLine.squared_length() );

    double distanceFromAbLine = sqrt( CGAL::squared_distance( position2D, positionProjectedToAbLine ) );

    double moduloDistanceFromAbLine = std::floor( distanceFromAbLine / implementWidth ) * implementWidth;

    std::cout << "GlobalPlanner::createPlanAB: abLine:" << abLine << ", implementWidth:" << implementWidth << ", distanceFromAbLine:" << distanceFromAbLine <<
              ", moduloDistanceFromAbLine:" << moduloDistanceFromAbLine << std::endl;


    for( int offsetCount = -pathsToGenerate ; offsetCount <= pathsToGenerate ; ++offsetCount ) {
      double offsetDistance = moduloDistanceFromAbLine + offsetCount * implementWidth;

      if( ab2D.has_on_positive_side( position2D ) ) {
        offsetDistance = -offsetDistance;
      }

      auto offsetVector = polarOffset( angleAbRad, offsetDistance );

      std::cout << "makeOffsettedSegment: offsetCount:" << offsetCount << ", offsetDistance:" << offsetDistance << ", offsetVector: " << offsetVector << ", angleAB:" << angleAB << std::endl;

      auto newLine = std::make_shared<PathPrimitiveLine>(
                       Line_2( ab2dSegment.source() - offsetVector, ab2dSegment.target() - offsetVector ),
                       implementWidth, true, offsetCount );

      if( !isLineAlreadyInPlan( newLine ) ) {
        plan.plan->push_back( newLine );
      }

    }

    qDebug() << "plan->size()" << plan.plan->size();
    sortPlan();
    emit planChanged( plan );
  }
}

GlobalPlannerLines::GlobalPlannerLines( QWidget* mainWindow, Qt3DCore::QEntity* rootEntity, GeographicConvertionWrapper* tmw )
  : BlockBase(),
    mainWindow( mainWindow ), tmw( tmw ) {
  // a point marker -> orange
  {
    aPointEntity = new Qt3DCore::QEntity( rootEntity );

    aPointMesh = new Qt3DExtras::QSphereMesh( aPointEntity );
    aPointMesh->setRadius( .2f );
    aPointMesh->setSlices( 20 );
    aPointMesh->setRings( 20 );

    aPointTransform = new Qt3DCore::QTransform( aPointEntity );

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial( aPointEntity );
    material->setDiffuse( QColor( "orange" ) );

    aPointEntity->addComponent( aPointMesh );
    aPointEntity->addComponent( material );
    aPointEntity->addComponent( aPointTransform );
    aPointEntity->setEnabled( false );

    aTextEntity = new Qt3DCore::QEntity( aPointEntity );
    Qt3DExtras::QExtrudedTextMesh* aTextMesh = new Qt3DExtras::QExtrudedTextMesh( aTextEntity );
    aTextMesh->setText( QStringLiteral( "A" ) );
    aTextMesh->setDepth( 0.05f );

    aTextEntity->setEnabled( true );
    aTextTransform = new Qt3DCore::QTransform( aTextEntity );
    aTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
    aTextTransform->setScale( 2.0f );
    aTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
    aTextEntity->addComponent( aTextTransform );
    aTextEntity->addComponent( aTextMesh );
    aTextEntity->addComponent( material );
  }

  // b point marker -> purple
  {
    bPointEntity = new Qt3DCore::QEntity( rootEntity );
    bPointMesh = new Qt3DExtras::QSphereMesh( bPointEntity );
    bPointMesh->setRadius( .2f );
    bPointMesh->setSlices( 20 );
    bPointMesh->setRings( 20 );

    bPointTransform = new Qt3DCore::QTransform( bPointEntity );

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial( bPointEntity );
    material->setDiffuse( QColor( "purple" ) );

    bPointEntity->addComponent( bPointMesh );
    bPointEntity->addComponent( material );
    bPointEntity->addComponent( bPointTransform );
    bPointEntity->setEnabled( false );

    bTextEntity = new Qt3DCore::QEntity( bPointEntity );
    Qt3DExtras::QExtrudedTextMesh* bTextMesh = new Qt3DExtras::QExtrudedTextMesh( bTextEntity );
    bTextMesh->setText( QStringLiteral( "B" ) );
    bTextMesh->setDepth( 0.05f );

    bTextEntity->setEnabled( true );
    bTextTransform = new Qt3DCore::QTransform( bTextEntity );
    bTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
    bTextTransform->setScale( 2.0f );
    bTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
    bTextEntity->addComponent( bTextTransform );
    bTextEntity->addComponent( bTextMesh );
    bTextEntity->addComponent( material );
  }

  // test for recording
  {
    m_baseEntity = new Qt3DCore::QEntity( rootEntity );
    m_baseTransform = new Qt3DCore::QTransform( m_baseEntity );
    m_baseEntity->addComponent( m_baseTransform );

    m_pointsEntity = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity2 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity3 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity4 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity->setEnabled( false );
    m_segmentsEntity2->setEnabled( false );
    m_segmentsEntity3->setEnabled( false );

    m_pointsMesh = new BufferMesh( m_pointsEntity );
    m_pointsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Points );
    m_pointsEntity->addComponent( m_pointsMesh );

    m_segmentsMesh = new BufferMesh( m_segmentsEntity );
    m_segmentsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
    m_segmentsEntity->addComponent( m_segmentsMesh );

    m_segmentsMesh2 = new BufferMesh( m_segmentsEntity2 );
    m_segmentsMesh2->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );
    m_segmentsEntity2->addComponent( m_segmentsMesh2 );

    m_segmentsMesh3 = new BufferMesh( m_segmentsEntity3 );
    m_segmentsMesh3->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );
    m_segmentsEntity3->addComponent( m_segmentsMesh3 );

    m_segmentsMesh4 = new BufferMesh( m_segmentsEntity4 );
    m_segmentsMesh4->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );
    m_segmentsEntity4->addComponent( m_segmentsMesh4 );

    m_pointsMaterial = new Qt3DExtras::QPhongMaterial( m_pointsEntity );
    m_segmentsMaterial = new Qt3DExtras::QPhongMaterial( m_segmentsEntity );
    m_segmentsMaterial2 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity2 );
    m_segmentsMaterial3 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity3 );
    m_segmentsMaterial4 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity4 );

    m_pointsMaterial->setAmbient( Qt::yellow );
    m_segmentsMaterial->setAmbient( Qt::green );
    m_segmentsMaterial2->setAmbient( Qt::green );
    m_segmentsMaterial3->setAmbient( Qt::blue );
    m_segmentsMaterial4->setAmbient( Qt::red );

    m_pointsEntity->addComponent( m_pointsMaterial );
    m_segmentsEntity->addComponent( m_segmentsMaterial );
    m_segmentsEntity2->addComponent( m_segmentsMaterial2 );
    m_segmentsEntity3->addComponent( m_segmentsMaterial3 );
    m_segmentsEntity4->addComponent( m_segmentsMaterial4 );
  }

  // create the CGAL worker and move to it's own thread
  {
    threadForCgalWorker = new CgalThread( this );
    cgalWorker = new CgalWorker();
    cgalWorker->moveToThread( threadForCgalWorker );

    qRegisterMetaType<FieldsOptimitionToolbar::AlphaType>();
    qRegisterMetaType<uint32_t>( "uint32_t" );

//    QObject::connect( this, &GlobalPlanner::requestNewRunNumber, threadForCgalWorker, &CgalThread::requestNewRunNumber );
//    QObject::connect( threadForCgalWorker, &CgalThread::runNumberChanged, this, &GlobalPlanner::setRunNumber );
//    QObject::connect( threadForCgalWorker, &QThread::finished, cgalWorker, &CgalWorker::deleteLater );

//    QObject::connect( this, &GlobalPlanner::requestFieldOptimition, cgalWorker, &CgalWorker::fieldOptimitionWorker );
//    QObject::connect( cgalWorker, &CgalWorker::alphaChanged, this, &GlobalPlanner::alphaChanged );
//    QObject::connect( cgalWorker, &CgalWorker::fieldStatisticsChanged, this, &GlobalPlanner::fieldStatisticsChanged );
//    QObject::connect( cgalWorker, &CgalWorker::alphaShapeFinished, this, &GlobalPlanner::alphaShapeFinished );

    threadForCgalWorker->start();
  }

  Segment_2 segment( Point_2( 2, 2 ), Point_2( 2.2, 5 ) );
  PathPrimitiveSegment line1( segment, 1, true, 0 );
  PathPrimitiveLine line2( segment.supporting_line(), 1, true, 0 );
  Point_2 point( 1, 3 );
  qDebug() << "line1: " << line1.distanceToPoint( point );
  qDebug() << "line2: " << line2.distanceToPoint( point );

  Line_2 line3( Point_2( 2.2, 2.2 ), Point_2( 3, 3 ) );
  Segment_2 line4( Point_2( -1, 0 ), Point_2( 3, 3 ) );
  qDebug() << "l3:" << sqrt( CGAL::squared_distance( line3, point ) ) << "l4:" << sqrt( CGAL::squared_distance( line4, point ) );
}

#include "moc_GlobalPlannerLines.cpp"
