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

#pragma once

#include "../gui/OrientationBlockModel.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"
#include "../kinematic/PoseOptions.h"


#include <QObject>

#include "BlockBase.h"

// algorythm from here: http://tbirdal.blogspot.com/2019/10/i-allocate-this-post-to-providing.html

class OrientationBlock : public BlockBase {
    Q_OBJECT

  public:
    explicit OrientationBlock() {}

    void emitConfigSignals() override {
      emit orientationChanged( orientation );
    }

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;
      valuesObject[QStringLiteral( "X" )] = orientation.x();
      valuesObject[QStringLiteral( "Y" )] = orientation.y();
      valuesObject[QStringLiteral( "Z" )] = orientation.z();
      valuesObject[QStringLiteral( "W" )] = orientation.w();
      json[QStringLiteral( "values" )] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        if( valuesObject[QStringLiteral( "X" )].isDouble() ) {
          orientation.x() = valuesObject[QStringLiteral( "X" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "Y" )].isDouble() ) {
          orientation.y() = valuesObject[QStringLiteral( "Y" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "Z" )].isDouble() ) {
          orientation.z() = valuesObject[QStringLiteral( "Z" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "W" )].isDouble() ) {
          orientation.w() = valuesObject[QStringLiteral( "W" )].toDouble();
        }
      }
    }

  public slots:
    void setAveragerEnabled( bool enabled ) {
      averagerEnabledOld = averagerEnabled;
      averagerEnabled = enabled;

      if( averagerEnabledOld != averagerEnabled ) {
        if( averagerEnabled == true ) {
          A.setZero();
          numMeasurements = 0;
          positionStart = position;
        } else {
          A = ( 1.0 / numMeasurements ) * A;

          Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig( A );

          Eigen::Vector4d qavg = eig.eigenvectors().col( 3 );

          orientation = Eigen::Quaterniond( qavg );

          if( position != positionStart ) {
            auto eulers = quaternionToEuler( orientation );
            double heading = std::atan2( position.y() - positionStart.y(),
                                         position.x() - positionStart.x() );
            qDebug() << eulers.x() << eulers.y() << heading;
            orientation = eulerToQuaternion( eulers.x(), eulers.y(), heading );
          }

          emit orientationChanged( orientation );
        }
      }
    }

    void setOrientation( const Eigen::Quaterniond orientation ) {
      if( averagerEnabled ) {
        ++numMeasurements;
        Eigen::Vector4d q( orientation.x(), orientation.y(), orientation.z(), orientation.w() );

        if( q[0] < 0 ) {
          q = -q;
        }

        A = q * q.adjoint() + A;
      }
    }

    void setPose( const Point_3 position, Eigen::Quaterniond orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;

        if( averagerEnabled ) {
          ++numMeasurements;
          Eigen::Vector4d q( orientation.x(), orientation.y(), orientation.z(), orientation.w() );

          if( q[0] < 0 ) {
            q = -q;
          }

          A = q * q.adjoint() + A;
        }
      }
    }

  signals:
    void orientationChanged( Eigen::Quaterniond );

  public:
    Eigen::Quaterniond orientation;
    Point_3 position = Point_3( 0, 0, 0 );
    Point_3 positionStart = Point_3( 0, 0, 0 );

    int numMeasurements = 0;
    bool averagerEnabled = false;
    bool averagerEnabledOld = false;

  private:
    Eigen::Matrix4d A;
};

class OrientationBlockFactory : public BlockFactory {
    Q_OBJECT

  public:
    OrientationBlockFactory( OrientationBlockModel* model )
      : BlockFactory(),
        model( model ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Orientation" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new OrientationBlock();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Averager Enabled" ), QLatin1String( SLOT( setAveragerEnabled( bool ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( const Eigen::Quaterniond ) ) ) );

      b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( Eigen::Quaterniond ) ) ) );

      b->setBrush( valueColor );

      model->resetModel();

      return b;
    }

  private:
    OrientationBlockModel* model = nullptr;
};
