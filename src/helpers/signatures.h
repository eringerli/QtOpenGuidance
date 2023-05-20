// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// clang-format off

#define ACTION_SIGNATURE_SIGNAL const bool
#define ACTION_SIGNATURE_SLOT   const bool
#define ACTION_SIGNATURE        const bool

#define NUMBER_SIGNATURE_SIGNAL const double,const CalculationOption::Options
#define NUMBER_SIGNATURE_SLOT   const double,const CalculationOption::Options=CalculationOption::Option::None
#define NUMBER_SIGNATURE        const double,const CalculationOption::Options

#define ORIENTATION_SIGNATURE_SIGNAL const Eigen::Quaterniond&,const CalculationOption::Options
#define ORIENTATION_SIGNATURE_SLOT   const Eigen::Quaterniond&,const CalculationOption::Options=CalculationOption::Option::None
#define ORIENTATION_SIGNATURE        const Eigen::Quaterniond&,const CalculationOption::Options

#define VECTOR_SIGNATURE_SIGNAL const Eigen::Vector3d&,const CalculationOption::Options
#define VECTOR_SIGNATURE_SLOT   const Eigen::Vector3d&,const CalculationOption::Options=CalculationOption::Option::None
#define VECTOR_SIGNATURE        const Eigen::Vector3d&,const CalculationOption::Options

#define POSE_SIGNATURE_SIGNAL const Eigen::Vector3d&,const Eigen::Quaterniond&,const CalculationOption::Options
#define POSE_SIGNATURE_SLOT \
  const Eigen::Vector3d&,const Eigen::Quaterniond&,const CalculationOption::Options=CalculationOption::Option::None
#define POSE_SIGNATURE const Eigen::Vector3d&,const Eigen::Quaterniond&,const CalculationOption::Options

#define IMU_SIGNATURE_SIGNAL const double,const Eigen::Vector3d&,const Eigen::Vector3d&,const Eigen::Vector3d&
#define IMU_SIGNATURE_SLOT   const double,const Eigen::Vector3d&,const Eigen::Vector3d&,const Eigen::Vector3d&
#define IMU_SIGNATURE        const double,const Eigen::Vector3d&,const Eigen::Vector3d&,const Eigen::Vector3d&

// clang-format on

// , CalculationOption::Options

// , CalculationOption::Option::NoOption

// , CalculationOption::Options options ) { if( !options.testFlag( CalculationOption::Option::NoGraphics ) ) {

// , CalculationOption::Options options ) { if( !options.testFlag( CalculationOption::Option::NoControl ) ) {

// , CalculationOption::Options options ) { if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
