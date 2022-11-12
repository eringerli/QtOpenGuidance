// Copyright( C ) 2022 Christian Riggenbach
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

#define ACTION_SIGNATURE_SIGNAL const bool
#define ACTION_SIGNATURE_SLOT   const bool
#define ACTION_SIGNATURE        const bool

#define NUMBER_SIGNATURE_SIGNAL const double, const CalculationOption::Options
#define NUMBER_SIGNATURE_SLOT   const double, const CalculationOption::Options = CalculationOption::Option::None
#define NUMBER_SIGNATURE        const double, const CalculationOption::Options

// , CalculationOption::Options

// , CalculationOption::Option::NoOption

// , CalculationOption::Options options ) { if( !options.testFlag( CalculationOption::Option::NoGraphics ) ) {

// , CalculationOption::Options options ) { if( !options.testFlag( CalculationOption::Option::NoControl ) ) {

// , CalculationOption::Options options ) { if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {

#define POSE_SIGNATURE_SIGNAL const Eigen::Vector3d&, const Eigen::Quaterniond&, const CalculationOption::Options
#define POSE_SIGNATURE_SLOT \
  const Eigen::Vector3d&, const Eigen::Quaterniond&, const CalculationOption::Options = CalculationOption::Option::None
#define POSE_SIGNATURE const Eigen::Vector3d&, const Eigen::Quaterniond&, const CalculationOption::Options
