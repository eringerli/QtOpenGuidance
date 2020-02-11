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

#ifndef GlobalPlannerModel_H
#define GlobalPlannerModel_H

#include <QObject>

#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>
#include <Qt3DRender/QAttribute>

#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"

#include "../3d/ArrowTexture.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>


class GlobalPlannerModel : public BlockBase {
    Q_OBJECT

  public:
    explicit GlobalPlannerModel( Qt3DCore::QEntity* rootEntity )
      : BlockBase(),
        rootEntity( rootEntity ) {

      // arrows for passes
      {
        arrowsEntity = new Qt3DCore::QEntity( rootEntity );
        arrowsTransform = new Qt3DCore::QTransform( arrowsEntity );
        arrowsEntity->addComponent( arrowsTransform );

        arrowsForegroundEntity = new Qt3DCore::QEntity( arrowsEntity );

        arrowsForegroundGeometryRenderer = new Qt3DRender::QGeometryRenderer( arrowsForegroundEntity );
        arrowsForegroundEntity->addComponent( arrowsForegroundGeometryRenderer );

        arrowsForegroundGeometry = new Qt3DRender::QGeometry( arrowsForegroundGeometryRenderer );
        arrowsForegroundGeometryRenderer->setGeometry( arrowsForegroundGeometry );

        arrowsForegroundGeometryRenderer->setInstanceCount( 1 );
        arrowsForegroundGeometryRenderer->setIndexOffset( 0 );
        arrowsForegroundGeometryRenderer->setFirstInstance( 0 );
        arrowsForegroundGeometryRenderer->setPrimitiveType( Qt3DRender::QGeometryRenderer::Triangles );

        // Vertex positions
        {
          arrowsForegroundPositionAttribute = new Qt3DRender::QAttribute( arrowsForegroundGeometry );
          arrowsForegroundPositionAttribute->setName( Qt3DRender::QAttribute::defaultPositionAttributeName() );
          arrowsForegroundPositionAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );

          arrowsForegroundVertexBuffer = new Qt3DRender::QBuffer( arrowsForegroundPositionAttribute );
          arrowsForegroundPositionAttribute->setBuffer( arrowsForegroundVertexBuffer );

#if QT_VERSION >= 0x050800
          arrowsForegroundPositionAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
          arrowsForegroundPositionAttribute->setVertexSize( 3 );
#else
          arrowsForegroundPositionAttribute->setDataType( Qt3DRender::QAttribute::Float );
          arrowsForegroundPositionAttribute->setDataSize( 3 );
#endif

          arrowsForegroundGeometry->addAttribute( arrowsForegroundPositionAttribute );
        }

        // indices
        {
          arrowsForegroundIndicesAttribute = new Qt3DRender::QAttribute( arrowsForegroundGeometry );
          arrowsForegroundIndicesAttribute->setAttributeType( Qt3DRender::QAttribute::IndexAttribute );

          arrowsForegroundIndicesBuffer = new Qt3DRender::QBuffer( arrowsForegroundIndicesAttribute );
          arrowsForegroundIndicesAttribute->setBuffer( arrowsForegroundIndicesBuffer );

#if QT_VERSION >= 0x050800
          arrowsForegroundIndicesAttribute->setVertexBaseType( Qt3DRender::QAttribute::UnsignedShort );
#else
          arrowsForegroundIndicesAttribute->setDataType( Qt3DRender::QAttribute::UnsignedInt );
#endif

          arrowsForegroundGeometry->addAttribute( arrowsForegroundIndicesAttribute );
        }

        // texture coordinates
        {
          arrowsForegroundTextureCoordinatesAttribute = new Qt3DRender::QAttribute( arrowsForegroundGeometry );
          arrowsForegroundTextureCoordinatesAttribute->setName( Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName() );
          arrowsForegroundTextureCoordinatesAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );

          arrowsForegroundTextureCoordinatesBuffer = new Qt3DRender::QBuffer( arrowsForegroundTextureCoordinatesAttribute );
          arrowsForegroundTextureCoordinatesAttribute->setBuffer( arrowsForegroundTextureCoordinatesBuffer );

#if QT_VERSION >= 0x050800
          arrowsForegroundTextureCoordinatesAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
          arrowsForegroundTextureCoordinatesAttribute->setVertexSize( 2 );
#else
          arrowsForegroundTextureCoordinatesAttribute->setDataType( Qt3DRender::QAttribute::Float );
          arrowsForegroundTextureCoordinatesAttribute->setDataSize( 2 );
#endif

          arrowsForegroundGeometry->addAttribute( arrowsForegroundTextureCoordinatesAttribute );
        }

        arrowsForegroundDiffuseSpecularMaterial = new Qt3DExtras::QDiffuseSpecularMaterial( arrowsForegroundEntity );
        arrowsForegroundTexture = new Qt3DRender::QTexture2D( arrowsForegroundDiffuseSpecularMaterial );
        arrowsForegroundArrowTexture = new ArrowTexture( arrowsForegroundTexture );

        arrowsForegroundTexture->addTextureImage( arrowsForegroundArrowTexture );
        arrowsForegroundTexture->setWrapMode( Qt3DRender::QTextureWrapMode( Qt3DRender::QTextureWrapMode::WrapMode::Repeat,
                                              Qt3DRender::QTextureWrapMode::WrapMode::Repeat,
                                              Qt3DRender::QTextureWrapMode::WrapMode::Repeat ) );
        arrowsForegroundTexture->setMagnificationFilter( Qt3DRender::QAbstractTexture::Linear );
        arrowsForegroundTexture->setMinificationFilter( Qt3DRender::QAbstractTexture::LinearMipMapLinear );
        arrowsForegroundTexture->setGenerateMipMaps( true );
        arrowsForegroundTexture->setMaximumAnisotropy( 16 );
        qDebug() << "maximumAnisotropy" << arrowsForegroundTexture->maximumAnisotropy();

        arrowsForegroundDiffuseSpecularMaterial->setDiffuse( QVariant::fromValue( arrowsForegroundTexture ) );
        arrowsForegroundDiffuseSpecularMaterial->setAlphaBlendingEnabled( true );
        arrowsForegroundDiffuseSpecularMaterial->setAmbient( QColor( 255, 255, 255, 127 ) );

        arrowsForegroundEntity->addComponent( arrowsForegroundDiffuseSpecularMaterial );

        setColors();
      }
    }

  public slots:
    void setVisible( bool visible ) {
      arrowsEntity->setEnabled( visible );
    }

    void setPlannerModelSettings( int arrowSize, int arrowSizeWidth, float textureSize, int centerLineSize, int borderLineSize,
                                  QColor arrowColor, QColor centerLineColor, QColor borderLineColor, QColor backgroundColor ) {
      this->textureSize = textureSize;
      this->arrowsForegroundArrowTexture->setSettings( arrowSize, arrowSizeWidth, centerLineSize, borderLineSize,
          arrowColor, centerLineColor, borderLineColor, backgroundColor );
      recalculateTextureCoordinates();
    }

    void setPlan( const QVector<QSharedPointer<PathPrimitive>>& plan ) {
      this->plan = plan;
      recalculateMeshes();
    }

    void setPose( const Point_3& position, const QQuaternion orientation, const PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation = orientation;
        arrowsTransform->setTranslation( QVector3D( 0, 0, float( position.z() ) ) );
      }
    }

    void recalculateMeshes() {
      QVector<QVector3D> positions;
      QVector<quint16> indices;
      QVector<QVector2D> textureCoordinates;
      bool anyDirection = false;

      for( const auto& primitive : qAsConst( plan ) ) {
        auto* line =  qobject_cast<PathPrimitiveLine*>( primitive.data() );

        if( line ) {
          qreal headingOfLine = line->line.angle();
          qreal width2 = qAbs( line->width / 2 );
          QLineF offsetLeft = QLineF::fromPolar( width2, headingOfLine + 90 );
          QLineF offsetRight = QLineF::fromPolar( width2, headingOfLine - 90 );

          QLineF tmpLineLeft = line->line.translated( offsetLeft.p2() );
          QLineF tmpLineRight = line->line.translated( offsetRight.p2() );

          if( line->anyDirection ) {
            anyDirection = true;
          }

          quint16 indexOffset = quint16( positions.size() );

          positions << QVector3D( float( tmpLineLeft.x1() ), float( tmpLineLeft.y1() ), 0 );
          positions << QVector3D( float( tmpLineLeft.x2() ), float( tmpLineLeft.y2() ), 0 );
          positions << QVector3D( float( tmpLineRight.x1() ), float( tmpLineRight.y1() ), 0 );
          positions << QVector3D( float( tmpLineRight.x2() ), float( tmpLineRight.y2() ), 0 );

          indices << indexOffset + 2;
          indices << indexOffset + 0;
          indices << indexOffset + 3;
          indices << indexOffset + 1;
          indices << indexOffset + 3;
          indices << indexOffset + 0;

          float textureCoordinateMaxY = float( line->line.length() ) / ( textureSize + distanceBetweenArrows );
//          arrowsForegroundArrowTexture->setDimensions();
          textureCoordinates << QVector2D( 0, 0 );
          textureCoordinates << QVector2D( textureCoordinateMaxY, 0 );
          textureCoordinates << QVector2D( 0, 1 );
          textureCoordinates << QVector2D( textureCoordinateMaxY, 1 );
        }
      }

      QByteArray positionsBufferData;
      positionsBufferData.resize( positions.size() * static_cast<int>( sizeof( QVector3D ) ) );
      memcpy( positionsBufferData.data(), positions.constData(), static_cast<size_t>( positions.size() * static_cast<int>( sizeof( QVector3D ) ) ) );
      arrowsForegroundVertexBuffer->setData( positionsBufferData );
      arrowsForegroundPositionAttribute->setCount( uint( positions.size() ) );

      QByteArray indicesBufferData;
      indicesBufferData.resize( indices.size() * static_cast<int>( sizeof( quint16 ) ) );
      memcpy( indicesBufferData.data(), indices.constData(), static_cast<size_t>( indices.size() * static_cast<int>( sizeof( quint16 ) ) ) );
      arrowsForegroundIndicesBuffer->setData( indicesBufferData );
      arrowsForegroundIndicesAttribute->setCount( uint( indices.size() ) );
      arrowsForegroundGeometryRenderer->setVertexCount( int( indices.size() ) );

      QByteArray textureCoordinatesBufferData;
      textureCoordinatesBufferData.resize( textureCoordinates.size() * static_cast<int>( sizeof( QVector2D ) ) );
      memcpy( textureCoordinatesBufferData.data(), textureCoordinates.constData(), static_cast<size_t>( textureCoordinates.size() ) * sizeof( QVector2D ) );
      arrowsForegroundTextureCoordinatesBuffer->setData( textureCoordinatesBufferData );
      arrowsForegroundTextureCoordinatesAttribute->setCount( uint( textureCoordinates.size() ) );

      arrowsForegroundArrowTexture->setAnyDirectionArrows( anyDirection );
    }

    void recalculateTextureCoordinates() {
      QVector<QVector2D> textureCoordinates;

      for( const auto& primitive : qAsConst( plan ) ) {
        auto* line =  qobject_cast<PathPrimitiveLine*>( primitive.data() );

        if( line ) {
          float textureCoordinateMaxY = float( line->line.length() ) / ( textureSize + distanceBetweenArrows );
          textureCoordinates << QVector2D( 0, 0 );
          textureCoordinates << QVector2D( textureCoordinateMaxY, 0 );
          textureCoordinates << QVector2D( 0, 1 );
          textureCoordinates << QVector2D( textureCoordinateMaxY, 1 );
        }
      }

      QByteArray textureCoordinatesBufferData;
      textureCoordinatesBufferData.resize( textureCoordinates.size() * static_cast<int>( sizeof( QVector2D ) ) );
      memcpy( textureCoordinatesBufferData.data(), textureCoordinates.constData(), static_cast<size_t>( textureCoordinates.size() ) * sizeof( QVector2D ) );
      arrowsForegroundTextureCoordinatesBuffer->setData( textureCoordinatesBufferData );
      arrowsForegroundTextureCoordinatesAttribute->setCount( uint( textureCoordinates.size() ) );
    }

  private:
    void setColors() {
    }

  public:
    Point_3 position = Point_3();
    QQuaternion orientation = QQuaternion();

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;

    // arrows
    Qt3DCore::QEntity* arrowsEntity = nullptr;
    Qt3DCore::QTransform* arrowsTransform = nullptr;

    Qt3DCore::QEntity* arrowsForegroundEntity = nullptr;
    Qt3DRender::QGeometry* arrowsForegroundGeometry = nullptr;
    Qt3DRender::QGeometryRenderer* arrowsForegroundGeometryRenderer = nullptr;
    Qt3DRender::QAttribute* arrowsForegroundPositionAttribute = nullptr;
    Qt3DRender::QAttribute* arrowsForegroundIndicesAttribute = nullptr;
    Qt3DRender::QAttribute* arrowsForegroundTextureCoordinatesAttribute = nullptr;

    Qt3DRender::QBuffer* arrowsForegroundVertexBuffer = nullptr;
    Qt3DRender::QBuffer* arrowsForegroundIndicesBuffer = nullptr;
    Qt3DRender::QBuffer* arrowsForegroundTextureCoordinatesBuffer = nullptr;

    ArrowTexture* arrowsForegroundArrowTexture = nullptr;
    Qt3DRender::QTexture2D* arrowsForegroundTexture = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial* arrowsForegroundDiffuseSpecularMaterial = nullptr;

    // values of the arrows
    float textureSize = 3, distanceBetweenArrows = 3;

  private:
    QVector<QSharedPointer<PathPrimitive>> plan;

};

class GlobalPlannerModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    GlobalPlannerModelFactory( Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Global Planner Model" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene ) override {
      auto* obj = new GlobalPlannerModel( rootEntity );
      auto* b = createBaseBlock( scene, obj, true );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const QVector<QSharedPointer<PathPrimitive>>& ) ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};

#endif // GlobalPlannerModel_H

