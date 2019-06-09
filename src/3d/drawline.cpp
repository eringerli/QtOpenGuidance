// https://forum.qt.io/topic/66808/qt3d-draw-grid-axis-lines/3

// with fix from
// https://forum.qt.io/topic/66808/qt3d-draw-grid-axis-lines/5
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

void drawLine( const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity* _rootEntity ) {
  auto* geometry = new Qt3DRender::QGeometry( _rootEntity );

  // position vertices (start and end)
  QByteArray bufferBytes;
  bufferBytes.resize( 3 * 2 * sizeof( float ) ); // start.x, start.y, start.end + end.x, end.y, end.z
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
  positionAttribute->setCount( 2 );
  geometry->addAttribute( positionAttribute ); // We add the vertices in the geometry

  // connectivity between vertices
  QByteArray indexBytes;
  indexBytes.resize( 2 * sizeof( unsigned int ) ); // start to end
  unsigned int* indices = reinterpret_cast<unsigned int*>( indexBytes.data() );
  *indices++ = 0;
  *indices++ = 1;

  auto* indexBuffer = new Qt3DRender::QBuffer( Qt3DRender::QBuffer::IndexBuffer, geometry );
  indexBuffer->setData( indexBytes );

  auto* indexAttribute = new Qt3DRender::QAttribute( geometry );
  indexAttribute->setVertexBaseType( Qt3DRender::QAttribute::UnsignedInt );
  indexAttribute->setAttributeType( Qt3DRender::QAttribute::IndexAttribute );
  indexAttribute->setBuffer( indexBuffer );
  indexAttribute->setCount( 2 );
  geometry->addAttribute( indexAttribute ); // We add the indices linking the points in the geometry

  // mesh
  auto* line = new Qt3DRender::QGeometryRenderer( _rootEntity );
  line->setGeometry( geometry );
  line->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
  auto* material = new Qt3DExtras::QPhongMaterial( _rootEntity );
  material->setAmbient( color );

  // entity
  auto* lineEntity = new Qt3DCore::QEntity( _rootEntity );
  lineEntity->addComponent( line );
  lineEntity->addComponent( material );
}
