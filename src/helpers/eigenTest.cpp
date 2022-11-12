#include <iostream>

#include "anglesHelper.h"
#include "eigenHelper.h"

int
main( int argc, char** argv ) {
  Eigen::IOFormat CommaInitFmt( Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "(", ")" );

  {
    std::cout << "+X" << std::endl;
    auto orientation = Eigen::Quaterniond( Eigen::AngleAxisd( 0.25 * M_PI, Eigen::Vector3d::UnitX() ) );
    std::cout << "orientation: " << ( quaternionToTaitBryan( orientation ) * 180 / M_PI ).format( CommaInitFmt ) << std::endl;
  }
  {
    std::cout << "-X" << std::endl;
    auto orientation = Eigen::Quaterniond( Eigen::AngleAxisd( -0.25 * M_PI, Eigen::Vector3d::UnitX() ) );
    std::cout << "orientation: " << ( quaternionToTaitBryan( orientation ) * 180 / M_PI ).format( CommaInitFmt ) << std::endl;
  }
  {
    std::cout << "+Y" << std::endl;
    auto orientation = Eigen::Quaterniond( Eigen::AngleAxisd( 0.25 * M_PI, Eigen::Vector3d::UnitY() ) );
    std::cout << "orientation: " << ( quaternionToTaitBryan( orientation ) * 180 / M_PI ).format( CommaInitFmt ) << std::endl;
  }
  {
    std::cout << "+Y" << std::endl;
    auto orientation = Eigen::Quaterniond( Eigen::AngleAxisd( -0.25 * M_PI, Eigen::Vector3d::UnitY() ) );
    std::cout << "orientation: " << ( quaternionToTaitBryan( orientation ) * 180 / M_PI ).format( CommaInitFmt ) << std::endl;
  }
  {
    std::cout << "+Z" << std::endl;
    auto orientation = Eigen::Quaterniond( Eigen::AngleAxisd( 0.25 * M_PI, Eigen::Vector3d::UnitZ() ) );
    std::cout << "orientation: " << ( quaternionToTaitBryan( orientation ) * 180 / M_PI ).format( CommaInitFmt ) << std::endl;
  }
  {
    std::cout << "+Z" << std::endl;
    auto orientation = Eigen::Quaterniond( Eigen::AngleAxisd( -0.25 * M_PI, Eigen::Vector3d::UnitZ() ) );
    std::cout << "orientation: " << ( quaternionToTaitBryan( orientation ) * 180 / M_PI ).format( CommaInitFmt ) << std::endl;
  }

  {
    std::cout << "order" << std::endl;
    auto orientation = /*Eigen::Quaterniond(Eigen::AngleAxisd(0.1*M_PI,
                        * Eigen::Vector3d::UnitX()))
                        * Eigen::Quaterniond(Eigen::AngleAxisd(0.2*M_PI,
                        * Eigen::Vector3d::UnitY()))
                        * */
      Eigen::Quaterniond( Eigen::AngleAxisd( degreesToRadians( 90. ), Eigen::Vector3d::UnitZ() ) );
    std::cout << "orientation: " << ( radiansToDegrees( quaternionToTaitBryan( orientation ) ) ).format( CommaInitFmt ) << std::endl;

    {
      Eigen::EulerAnglesXYZd euler( orientation );
      std::cout << "xyz: " << ( radiansToDegrees( euler.angles() ) ).format( CommaInitFmt )
                << " | ypr: " << radiansToDegrees( getYaw( euler.angles() ) ) << ", " << radiansToDegrees( getPitch( euler.angles() ) )
                << ", " << radiansToDegrees( getRoll( euler.angles() ) ) << std::endl;
    }
    {
      Eigen::EulerAnglesYXZd euler( orientation );
      std::cout << "yxz: " << ( radiansToDegrees( euler.angles() ) ).format( CommaInitFmt )
                << " | ypr: " << radiansToDegrees( getYaw( euler.angles() ) ) << ", " << radiansToDegrees( getPitch( euler.angles() ) )
                << ", " << radiansToDegrees( getRoll( euler.angles() ) ) << std::endl;
    }
    {
      Eigen::EulerAnglesZYXd euler( orientation );
      std::cout << "zyx: " << ( radiansToDegrees( euler.angles() ) ).format( CommaInitFmt )
                << " | ypr: " << radiansToDegrees( getYaw( euler.angles() ) ) << ", " << radiansToDegrees( getPitch( euler.angles() ) )
                << ", " << radiansToDegrees( getRoll( euler.angles() ) ) << std::endl;
    }
    {
      Eigen::EulerAnglesZXYd euler( orientation );
      std::cout << "zxy: " << ( radiansToDegrees( euler.angles() ) ).format( CommaInitFmt )
                << " | ypr: " << radiansToDegrees( getYaw( euler.angles() ) ) << ", " << radiansToDegrees( getPitch( euler.angles() ) )
                << ", " << radiansToDegrees( getRoll( euler.angles() ) ) << std::endl;
    }
    {
      using MyTaitBryanSystem2 = Eigen::EulerSystem< Eigen::EULER_Z, -Eigen::EULER_Y, Eigen::EULER_X >;
      using MyTaitBryanAngles2 = Eigen::EulerAngles< double, MyTaitBryanSystem2 >;

      {
        MyTaitBryanAngles2 euler( orientation );
        std::cout << "+z-y+x: " << ( radiansToDegrees( euler.angles() ) ).format( CommaInitFmt )
                  << " | ypr: " << radiansToDegrees( getYaw( euler.angles() ) ) << ", " << radiansToDegrees( getPitch( euler.angles() ) )
                  << ", " << radiansToDegrees( getRoll( euler.angles() ) ) << std::endl;
      }

      {
        MyTaitBryanAngles2 euler( 10 * M_PI / 180, 20 * M_PI / 180, 30 * M_PI / 180 );
        std::cout << "+z-y+x: " << ( radiansToDegrees( euler.angles() ) ).format( CommaInitFmt )
                  << " | ypr: " << radiansToDegrees( getYaw( euler.angles() ) ) << ", " << radiansToDegrees( getPitch( euler.angles() ) )
                  << ", " << radiansToDegrees( getRoll( euler.angles() ) ) << std::endl;
      }
      {
        Eigen::Vector3d vec;
        getYaw( vec )   = 10;
        getPitch( vec ) = 20;
        getRoll( vec )  = 30;
        vec             = degreesToRadians( vec );

        MyTaitBryanAngles2 euler( vec.x(), vec.y(), vec.z() );
        std::cout << "+z-y+x: " << ( radiansToDegrees( euler.angles() ) ).format( CommaInitFmt )
                  << " | ypr: " << radiansToDegrees( getYaw( euler.angles() ) ) << ", " << radiansToDegrees( getPitch( euler.angles() ) )
                  << ", " << radiansToDegrees( getRoll( euler.angles() ) ) << std::endl;
      }
    }
    {
      auto orientation = taitBryanToQuaternion( 105 * M_PI / 180, 10 * M_PI / 180, -20 * M_PI / 180 );
      std::cout << "orientation: " << ( radiansToDegrees( quaternionToTaitBryan( orientation ) ) ).format( CommaInitFmt ) << std::endl;
    }
  }
}
