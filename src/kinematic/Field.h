#ifndef FIELD_H
#define FIELD_H


#include <QObject>

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

#include <QList>
#include <QVector>

#include <QRectF>
#include <QVector3D>
#include <QQuaternion>

#include <QtMath>
#include <QtGlobal>

#include <QDebug>

#include "../block/GuidanceBase.h"

class Tile;
class Track;

constexpr quint16 sizeOfTile = 50;
constexpr quint16 numPointsInTrack = 256;
constexpr quint16 calculateBoundingBoxEveryNumPose = 10;

class Track {
  public:
    Track( Qt3DCore::QEntity* rootEntity ) {
      positions.resize( numPointsInTrack * 3 * sizeof( float ) ); // 3 floats = 1 Vector3D
      orientations.resize( numPointsInTrack * 4 * sizeof( float ) ); // 4 floats = 1 Quaternion

      this->rootEntity = rootEntity;

      // qt3d-entity to display the points
      auto* geometry = new Qt3DRender::QGeometry( rootEntity );

      positionBuffer = new Qt3DRender::QBuffer( Qt3DRender::QBuffer::VertexBuffer, geometry );
      positionBuffer->setData( positions );

      positionAttribute = new Qt3DRender::QAttribute( geometry );
      positionAttribute->setName( Qt3DRender::QAttribute::defaultPositionAttributeName() );
      positionAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
      positionAttribute->setVertexSize( 3 );
      positionAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );
      positionAttribute->setBuffer( positionBuffer );
      positionAttribute->setByteStride( 3 * sizeof( float ) );
      positionAttribute->setCount( 0 );
      geometry->addAttribute( positionAttribute ); // We add the vertices in the geometry

      // mesh
      auto* line = new Qt3DRender::QGeometryRenderer( rootEntity );
      line->setGeometry( geometry );
      line->setPrimitiveType( Qt3DRender::QGeometryRenderer::Points );

      auto* material = new Qt3DExtras::QPhongMaterial( rootEntity );
      material->setAmbient( Qt::darkRed );

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
      auto* lineEntity = new Qt3DCore::QEntity( rootEntity );
      lineEntity->addComponent( line );
      lineEntity->addComponent( material );

      // we get a pointer to the data in the QByteArrays: it should no change, as we reserved the size
      currentPosPointer = reinterpret_cast<float*>( positions.data() );
      currentOrientationPointer = reinterpret_cast<float*>( orientations.data() );

      qDebug() << "Track()";
    }

//    // copy constructor
//    Track( const Track& obj ) {
//      this->boundingBox = obj.boundingBox;
//      this->positions = obj.positions;
//      this->orientations = obj.orientations;

//      this->currentPosPointer = obj.currentPosPointer;
//      this->currentOrientationPointer = obj.currentOrientationPointer;

//      this->numSavedPose = obj.numSavedPose;
//    }

    // returns true if this track is full and a new track should be started
    bool addNewPose( QVector3D position, QQuaternion orientation ) {
      *currentPosPointer++ = position.x();
      *currentPosPointer++ = position.y();
      *currentPosPointer++ = position.z();

      *currentOrientationPointer++ = orientation.scalar();
      *currentOrientationPointer++ = orientation.x();
      *currentOrientationPointer++ = orientation.y();
      *currentOrientationPointer++ = orientation.z();

//      positionBuffer->setData( positions );
      positionBuffer->updateData( 0, positions );
      positionAttribute->setCount( ++numSavedPose );

      qDebug() << position << numSavedPose;

      return numSavedPose > numPointsInTrack;
    }

    QVector3D getPositionAt( quint16 i ) {
      float* posPointer = reinterpret_cast<float*>( positions.data() );
      posPointer += i * 3;

      QVector3D vector;
      vector.setX( *posPointer++ );
      vector.setY( *posPointer++ );
      vector.setZ( *posPointer );
      return vector;
    }

    QQuaternion getOrientationnAt( quint16 i ) {
      float* orientationPointer = reinterpret_cast<float*>( positions.data() );
      orientationPointer += i * 4;

      QQuaternion orientation;
      orientation.setScalar( *orientationPointer++ );
      orientation.setX( *orientationPointer++ );
      orientation.setY( *orientationPointer++ );
      orientation.setZ( *orientationPointer );
      return orientation;
    }

    void calculateBoundingBox() {
      float xMin = 0,
            xMax = 0,
            yMin = sizeOfTile,
            yMax = sizeOfTile;

      float* posPointer = reinterpret_cast<float*>( positions.data() );

      for( quint16 i = 0; i < numSavedPose; i++ ) {
        xMin = qMin( xMin, *posPointer );
        xMax = qMax( xMax, *posPointer++ );

        yMin = qMin( yMin, *posPointer );
        yMax = qMax( yMax, *posPointer );

        // jump over QVector3D.z()
        posPointer += 2;
      }

      boundingBox.setTop( qreal( yMax ) );
      boundingBox.setBottom( qreal( yMin ) );
      boundingBox.setRight( qreal( xMax ) );
      boundingBox.setLeft( qreal( xMin ) );
    }

  public:
    Qt3DCore::QEntity* rootEntity = nullptr;
    Qt3DRender::QAttribute* positionAttribute = nullptr;
    Qt3DRender::QBuffer* positionBuffer = nullptr;

    QRectF boundingBox;
    QByteArray positions;
    QByteArray orientations;

    float* currentPosPointer = nullptr;
    float* currentOrientationPointer = nullptr;

    quint16 numSavedPose = 0;
};

class Tile {

  public:
    Tile( Qt3DCore::QEntity* rootEntity ) {

      this->rootEntity = rootEntity;

      baseTransform = new Qt3DCore::QTransform();
      baseEntity = new Qt3DCore::QEntity( rootEntity );
      baseEntity->addComponent( baseTransform );

      tracks += Track( baseEntity );
    }

    void addNewPose( QVector3D position, QQuaternion orientation ) {
      bool addNewTrack = tracks.last().addNewPose( position, orientation );

      if( addNewTrack ) {
        tracks += Track( baseEntity );
        qDebug() << "new Track";
      }
    }

  public:
    Qt3DCore::QEntity* rootEntity = nullptr;
    Qt3DCore::QEntity* baseEntity = nullptr;
    Qt3DCore::QTransform* baseTransform = nullptr;

    qint64 x;
    qint64 y;

    QList<Track> tracks;
};

class Field : public GuidanceBase {
    Q_OBJECT
  public:
    explicit Field( Qt3DCore::QEntity* rootEntity ) {
      this->rootEntity = rootEntity;
      baseTransform = new Qt3DCore::QTransform();
      baseEntity = new Qt3DCore::QEntity( rootEntity );
      baseEntity->addComponent( baseTransform );
    }

  signals:

  public slots:
    void addNewPose( QVector3D position, QQuaternion orientation ) {

      qDebug() << "Field::addNewPose()" << currentMinX << currentMaxX << currentMinY << currentMaxY << currentTileIndex << position;

      if( position.x() > currentMinX &&
          position.x() < currentMaxX &&
          position.y() > currentMinY &&
          position.y() < currentMaxY ) {
        qDebug() << "tiles[currentTileIndex].addNewPose( position, orientation )";
        tiles[currentTileIndex].addNewPose( position, orientation );
      } else {
        int tileIndex = 0;

        if( tiles.size() )
          for( ; tileIndex < tiles.size(); ++tileIndex ) {
            if( position.x() > tiles.at( tileIndex ).x &&
                position.x() < ( tiles.at( tileIndex ).x + sizeOfTile ) &&
                position.y() > tiles.at( tileIndex ).y &&
                position.y() < ( tiles.at( tileIndex ).y + sizeOfTile ) ) {
              currentTileIndex = tileIndex;
              qDebug() << "currentTileIndex = tileIndex";
            }
          }

        if( tileIndex == tiles.size() || tiles.size() == 0 ) {
          Tile tile( baseEntity );
          tile.x = qint64( std::floor( position.x() / sizeOfTile ) * sizeOfTile );
          tile.y = qint64( std::floor( position.y() / sizeOfTile ) * sizeOfTile );
          tiles += tile;

          currentMinY = tile.y;
          currentMinX = tile.x;
          currentMaxY = tile.y + sizeOfTile;
          currentMaxX = tile.x + sizeOfTile;

          currentTileIndex = tileIndex;
          qDebug() << "tileIndex == tiles.size()" << currentMinX << currentMaxX << currentMinY << currentMaxY << currentTileIndex;
        }

        tiles[currentTileIndex].addNewPose( position, orientation );
      }
    }

  public:
    QList<Tile> tiles;

    Qt3DCore::QEntity* rootEntity = nullptr;
    Qt3DCore::QEntity* baseEntity = nullptr;
    Qt3DCore::QTransform* baseTransform = nullptr;

    int currentTileIndex = -1;
    qint64 currentMinY = 0;
    qint64 currentMaxY = 0;
    qint64 currentMinX = 0;
    qint64 currentMaxX = 0;
};

class FieldFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    FieldFactory( Qt3DCore::QEntity* rootEntity )
      : GuidanceFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Field Storage" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new Field( rootEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Pose", SLOT( addNewPose( QVector3D, QQuaternion ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity;
};

#endif // FIELD_H
