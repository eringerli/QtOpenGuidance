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

#ifndef ACKERMANNSTEERING_H
#define ACKERMANNSTEERING_H

#include <QObject>

#include "BlockBase.h"

// all the formulas are from https://www.xarg.org/book/kinematics/ackerman-steering/

class AckermannSteering : public BlockBase {
    Q_OBJECT

  public:
    explicit AckermannSteering() = default;

  public slots:
    void setWheelbase( float wheelbase ) {
      m_wheelbase = wheelbase;
      m_correction = qreal( m_trackWidth / ( 2.0f * m_wheelbase ) );
    }
    void setTrackwidth( float trackWidth ) {
      m_trackWidth = trackWidth;
      m_correction = qreal( m_trackWidth / ( 2.0f * m_wheelbase ) );
    }

    void setSteeringAngle( float steerAngle ) {
      // as the cotangens (1/tan()) is used, there is no valid result for steerangle=0 -> filter it out
      if( qFuzzyIsNull( steerAngle ) ) {
        emit steeringAngleChanged( 0 );
        emit steeringAngleChangedLeft( 0 );
        emit steeringAngleChangedRight( 0 );
      } else {
        emit steeringAngleChanged( steerAngle );
        emit steeringAngleChangedLeft( float( qRadiansToDegrees( qAtan( 1 / ( ( 1 / qTan( qDegreesToRadians( qreal( steerAngle ) ) ) ) - m_correction ) ) ) ) );
        emit steeringAngleChangedRight( float( qRadiansToDegrees( qAtan( 1 / ( ( 1 / qTan( qDegreesToRadians( qreal( steerAngle ) ) ) ) + m_correction ) ) ) ) );
      }
    }

    void setSteeringAngleLeft( float steerAngle ) {
      // as the cotangens (1/tan()) is used, there is no valid result for steerangle=0 -> filter it out
      if( qFuzzyIsNull( steerAngle ) ) {
        emit steeringAngleChanged( 0 );
        emit steeringAngleChangedLeft( 0 );
        emit steeringAngleChangedRight( 0 );
      } else {
        emit steeringAngleChangedLeft( steerAngle );
        emit steeringAngleChanged( float( qRadiansToDegrees( qAtan( 1 / ( ( 1 / qTan( qDegreesToRadians( qreal( steerAngle ) ) ) ) + m_correction ) ) ) ) );
        emit steeringAngleChangedRight( float( qRadiansToDegrees( qAtan( 1 / ( ( 1 / qTan( qDegreesToRadians( qreal( steerAngle ) ) ) ) + ( 2 * m_correction ) ) ) ) ) );
      }
    }

    void setSteeringAngleRight( float steerAngle ) {
      // as the cotangens (1/tan()) is used, there is no valid result for steerangle=0 -> filter it out
      if( qFuzzyIsNull( steerAngle ) ) {
        emit steeringAngleChanged( 0 );
        emit steeringAngleChangedLeft( 0 );
        emit steeringAngleChangedRight( 0 );
      } else {
        emit steeringAngleChangedRight( steerAngle );
        emit steeringAngleChangedLeft( float( qRadiansToDegrees( qAtan( 1 / ( ( 1 / qTan( qDegreesToRadians( qreal( steerAngle ) ) ) ) - ( 2 * m_correction ) ) ) ) ) );
        emit steeringAngleChanged( float( qRadiansToDegrees( qAtan( 1 / ( ( 1 / qTan( qDegreesToRadians( qreal( steerAngle ) ) ) ) - m_correction ) ) ) ) );
      }
    }

  signals:
    void steeringAngleChanged( float );
    void steeringAngleChangedLeft( float );
    void steeringAngleChangedRight( float );

  private:
    float m_wheelbase = 2.4f;
    float m_trackWidth = 2.0f;

    // formula: m_trackWidth / ( 2.0f * m_wheelbase )
    qreal m_correction = 2 / ( 2 * 2.4 );
};

class AckermannSteeringFactory : public BlockFactory {
    Q_OBJECT

  public:
    AckermannSteeringFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Ackermann Steering" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene ) override {
      auto* obj = new AckermannSteering();
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( QStringLiteral( "Length Wheelbase" ), QLatin1String( SLOT( setWheelbase( float ) ) ) );
      b->addInputPort( QStringLiteral( "Track Width" ), QLatin1String( SLOT( setTrackwidth( float ) ) ) );

      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( float ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle Left" ), QLatin1String( SLOT( setSteeringAngleLeft( float ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle Right" ), QLatin1String( SLOT( setSteeringAngleRight( float ) ) ) );

      b->addOutputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SIGNAL( steeringAngleChanged( float ) ) ) );
      b->addOutputPort( QStringLiteral( "Steering Angle Left" ), QLatin1String( SIGNAL( steeringAngleChangedLeft( float ) ) ) );
      b->addOutputPort( QStringLiteral( "Steering Angle Right" ), QLatin1String( SIGNAL( steeringAngleChangedRight( float ) ) ) );

      return b;
    }

  private:
};

#endif // ACKERMANNSTEERING_H
