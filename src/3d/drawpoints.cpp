// https://forum.qt.io/topic/66808/qt3d-draw-grid-axis-lines/3

// with fix from
// https://forum.qt.io/topic/66808/qt3d-draw-grid-axis-lines/5
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QPointSize>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>

void drawPoints( const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity* _rootEntity ) {
  auto* geometry = new Qt3DRender::QGeometry( _rootEntity );

  // position vertices (start and end)
  QByteArray bufferBytes;
  bufferBytes.resize( 3 * 4 * sizeof( float ) ); // start.x, start.y, start.end + end.x, end.y, end.z
  float* positions = reinterpret_cast<float*>( bufferBytes.data() );
  *positions++ = start.x();
  *positions++ = start.y();
  *positions++ = start.z();
  *positions++ = end.x();
  *positions++ = end.y();
  *positions++ = end.z();

  auto* buf = new Qt3DRender::QBuffer( Qt3DRender::QBuffer::VertexBuffer, geometry );
  buf->setData( bufferBytes );

  auto* positionAttribute = new Qt3DRender::QAttribute( geometry );
  positionAttribute->setName( Qt3DRender::QAttribute::defaultPositionAttributeName() );
  positionAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
  positionAttribute->setVertexSize( 3 );
  positionAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );
  positionAttribute->setBuffer( buf );
  positionAttribute->setByteStride( 3 * sizeof( float ) );
  positionAttribute->setCount( 4 );
  geometry->addAttribute( positionAttribute ); // We add the vertices in the geometry

//  // connectivity between vertices
//  QByteArray indexBytes;
//  indexBytes.resize( 2 * sizeof( unsigned int ) ); // start to end
//  unsigned int* indices = reinterpret_cast<unsigned int*>( indexBytes.data() );
//  *indices++ = 0;
//  *indices++ = 1;

//  auto *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, geometry);
//  indexBuffer->setData( indexBytes );

//  auto* indexAttribute = new Qt3DRender::QAttribute( geometry );
//  indexAttribute->setVertexBaseType( Qt3DRender::QAttribute::UnsignedInt );
//  indexAttribute->setAttributeType( Qt3DRender::QAttribute::IndexAttribute );
//  indexAttribute->setBuffer( indexBuffer );
//  indexAttribute->setCount( 2 );
//  geometry->addAttribute( indexAttribute ); // We add the indices linking the points in the geometry

  // mesh
  auto* line = new Qt3DRender::QGeometryRenderer( _rootEntity );
  line->setGeometry( geometry );
  line->setPrimitiveType( Qt3DRender::QGeometryRenderer::Points );

  auto* material = new Qt3DExtras::QPhongMaterial( _rootEntity );
  material->setAmbient( color );

  // https://github.com/jclay/dev-journal/blob/master/draw-points-qt3d.md
  // this is my hacky way of setting point size
  // the better way to do this is probably to create
  // your own shader and then use QPointSize::SizeMode::Programmable
  // that's for another journal...
  auto effect = material->effect();

  for( auto t : effect->techniques() ) {
    for( auto rp : t->renderPasses() ) {
      auto pointSize = new Qt3DRender::QPointSize();
      pointSize->setSizeMode( Qt3DRender::QPointSize::SizeMode::Fixed );
      pointSize->setValue( 4.0f );
      rp->addRenderState( pointSize );
    }
  }

  // entity
  auto* lineEntity = new Qt3DCore::QEntity( _rootEntity );
  lineEntity->addComponent( line );
  lineEntity->addComponent( material );
}
