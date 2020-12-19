cmake_minimum_required(VERSION 3.16.0)

project(QtOpenGuidance VERSION 0.9.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include(cmake/modules/LDGOLD.cmake)

set(CGAL_DIR lib/CGAL-5.1.1/)
set(Eigen3_DIR lib/eigen)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Concurrent Widgets 3DCore 3DExtras 3DRender 3DInput PrintSupport)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(KDDockWidgets REQUIRED)
find_package(CGAL REQUIRED COMPONENTS Core)

########################################
# Find Space Navigator header and library
find_library(SPNAV_LIBRARY NAMES spnav)
find_file(SPNAV_HEADER NAMES spnav.h)
if (SPNAV_LIBRARY AND SPNAV_HEADER)
  message(STATUS "Looking for libspnav and spnav.h - found")
  set(HAVE_SPNAV TRUE)
else()
  message(STATUS "Looking for libspnav and spnav.h - not found")
  set(HAVE_SPNAV FALSE)
  set(SPNAV_LIBRARY "")
endif()

add_subdirectory(lib/geographiclib/ EXCLUDE_FROM_ALL)
add_subdirectory(lib/dubins_curves/ EXCLUDE_FROM_ALL)

# QCustomPlot
add_library(QCustomPlot
  lib/QCustomPlot/qcustomplot.cpp
  lib/QCustomPlot/qcustomplot.h
)
target_link_libraries(QCustomPlot
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::PrintSupport
)
target_compile_options(QCustomPlot PRIVATE -Wno-deprecated-declarations)

# qnodeseditor
add_library(qnodeseditor
  src/qnodeseditor/qneblock.cpp
  src/qnodeseditor/qneblock.h
  src/qnodeseditor/qneconnection.cpp
  src/qnodeseditor/qneconnection.h
  src/qnodeseditor/qnegestures.h
  src/qnodeseditor/qnegraphicsview.h
  src/qnodeseditor/qneport.cpp
  src/qnodeseditor/qneport.h
  src/qnodeseditor/qnodeseditor.cpp
  src/qnodeseditor/qnodeseditor.h
)
target_link_libraries(qnodeseditor
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
)
target_include_directories(qnodeseditor PRIVATE src/)
target_include_directories(qnodeseditor PRIVATE src/qnodeseditor/)

set(SOURCES
  src/3d/ArrowTexture.h
  src/3d/BufferMesh.cpp
  src/3d/BufferMeshGeometry.cpp
  src/3d/BufferMeshGeometry.h
  src/3d/BufferMeshGeometryWithNormal.cpp
  src/3d/BufferMeshGeometryWithNormal.h
  src/3d/BufferMesh.h
  src/3d/BufferMeshWithNormal.cpp
  src/3d/BufferMeshWithNormal.h
  src/3d/CultivatedAreaMesh.cpp
  src/3d/CultivatedAreaMeshGeometry.cpp
  src/3d/CultivatedAreaMeshGeometry.h
  src/3d/CultivatedAreaMesh.h
  src/3d/texturerendertarget.cpp
  src/3d/texturerendertarget.h
  src/block/arithmetic/ArithmeticAddition.h
  src/block/arithmetic/ArithmeticDivision.h
  src/block/arithmetic/ArithmeticMultiplication.h
  src/block/arithmetic/ArithmeticSubtraction.h
  src/block/base/DebugSink.h
  src/block/BlockBase.cpp
  src/block/BlockBase.h
  src/block/calculation/AckermannSteering.h
  src/block/calculation/AngularVelocityLimiter.h
  src/block/calculation/TransverseMercatorConverter.h
  src/block/comparison/ComparisonEqualTo.h
  src/block/comparison/ComparisonGreaterOrEqualTo.h
  src/block/comparison/ComparisonGreaterThan.h
  src/block/comparison/ComparisonLessOrEqualTo.h
  src/block/comparison/ComparisonLessThan.h
  src/block/comparison/ComparisonNotEqualTo.h
  src/block/converter/CommunicationJrk.h
  src/block/converter/CommunicationPgn7FFE.h
  src/block/converter/ValueTransmissionBase64Data.h
  src/block/converter/ValueTransmissionBase.cpp
  src/block/converter/ValueTransmissionBase.h
  src/block/converter/ValueTransmissionNumber.h
  src/block/converter/ValueTransmissionQuaternion.h
  src/block/converter/ValueTransmissionState.h
  src/block/dock/display/OrientationDockBlock.h
  src/block/dock/display/PositionDockBlock.h
  src/block/dock/display/ValueDockBlockBase.cpp
  src/block/dock/display/ValueDockBlockBase.h
  src/block/dock/display/ValueDockBlock.h
  src/block/dock/display/XteDockBlock.cpp
  src/block/dock/display/XteDockBlock.h
  src/block/dock/input/ActionDockBlock.cpp
  src/block/dock/input/ActionDockBlock.h
  src/block/dock/input/SliderDockBlock.cpp
  src/block/dock/input/SliderDockBlock.h
  src/block/dock/plot/OrientationPlotDockBlock.h
  src/block/dock/plot/PlotDockBlockBase.cpp
  src/block/dock/plot/PlotDockBlockBase.h
  src/block/dock/plot/ValuePlotDockBlock.h
  src/block/ExtendedKalmanFilter.h
  src/block/global/CameraController.h
  src/block/global/FieldManager.cpp
  src/block/global/FieldManager.h
  src/block/global/FpsMeasurement.h
  src/block/global/GlobalPlanner.cpp
  src/block/global/GlobalPlanner.h
  src/block/global/GridModel.h
  src/block/global/PoseSimulation.cpp
  src/block/global/PoseSimulation.h
  src/block/graphical/CultivatedAreaModel.cpp
  src/block/graphical/CultivatedAreaModel.h
  src/block/graphical/PathPlannerModel.cpp
  src/block/graphical/PathPlannerModel.h
  src/block/graphical/SprayerModel.cpp
  src/block/graphical/SprayerModel.h
  src/block/graphical/TractorModel.cpp
  src/block/graphical/TractorModel.h
  src/block/graphical/TrailerModel.cpp
  src/block/graphical/TrailerModel.h
  src/block/guidance/LocalPlanner.cpp
  src/block/guidance/LocalPlanner.h
  src/block/guidance/PoseSynchroniser.h
  src/block/guidance/StanleyGuidance.cpp
  src/block/guidance/StanleyGuidance.h
  src/block/guidance/XteGuidance.h
  src/block/kinematic/FixedKinematic.h
  src/block/kinematic/FixedKinematicPrimitive.h
  src/block/kinematic/TrailerKinematic.h
  src/block/kinematic/TrailerKinematicPrimitive.h
  src/block/literal/NumberObject.h
  src/block/literal/OrientationBlock.h
  src/block/literal/StringObject.h
  src/block/literal/VectorObject.h
  src/block/parser/NmeaParserGGA.h
  src/block/parser/NmeaParserHDT.h
  src/block/parser/NmeaParserRMC.h
  src/block/parser/UbxParser.h
  src/block/sectionControl/Implement.h
  src/block/sectionControl/ImplementSection.h
  src/block/sectionControl/SectionControl.cpp
  src/block/sectionControl/SectionControl.h
  src/block/stream/FileStream.h
  src/block/stream/UdpSocket.h
  src/filter/3wFRHRL/SystemModel.h
  src/filter/BicycleModel/ImuMeasurementModel.h
  src/filter/BicycleModel/PositionMeasurementModel.h
  src/filter/BicycleModel/SystemModel.h
  src/filter/KinematicModel/ImuMeasurementModel.h
  src/filter/KinematicModel/PositionMeasurementModel.h
  src/filter/KinematicModel/SystemModel.h
  src/filter/MeanMovingWindow.h
  src/gui/CameraToolbar.cpp
  src/gui/CameraToolbar.h
  src/gui/CameraToolbar.ui
  src/gui/dock/ActionDock.cpp
  src/gui/dock/ActionDock.h
  src/gui/dock/ActionDock.ui
  src/gui/dock/PlotDock.cpp
  src/gui/dock/PlotDock.h
  src/gui/dock/PlotDock.ui
  src/gui/dock/SliderDock.cpp
  src/gui/dock/SliderDock.h
  src/gui/dock/SliderDock.ui
  src/gui/dock/ThreeValuesDock.cpp
  src/gui/dock/ThreeValuesDock.h
  src/gui/dock/ThreeValuesDock.ui
  src/gui/dock/ValueDock.cpp
  src/gui/dock/ValueDock.h
  src/gui/dock/ValueDock.ui
  src/gui/dock/XteDock.cpp
  src/gui/dock/XteDock.h
  src/gui/dock/XteDock.ui
  src/gui/FieldsOptimitionToolbar.cpp
  src/gui/FieldsOptimitionToolbar.h
  src/gui/FieldsOptimitionToolbar.ui
  src/gui/FieldsToolbar.cpp
  src/gui/FieldsToolbar.h
  src/gui/FieldsToolbar.ui
  src/gui/FontComboboxDelegate.h
  src/gui/GlobalPlannerToolbar.cpp
  src/gui/GlobalPlannerToolbar.h
  src/gui/GlobalPlannerToolbar.ui
  src/gui/GuidanceToolbar.cpp
  src/gui/GuidanceToolbar.h
  src/gui/GuidanceToolbar.ui
  src/gui/GuidanceTurning.cpp
  src/gui/GuidanceTurning.h
  src/gui/GuidanceTurning.ui
  src/gui/model/ActionDockBlockModel.cpp
  src/gui/model/ActionDockBlockModel.h
  src/gui/model/ImplementBlockModel.cpp
  src/gui/model/ImplementBlockModel.h
  src/gui/model/ImplementSectionModel.cpp
  src/gui/model/ImplementSectionModel.h
  src/gui/model/NumberBlockModel.cpp
  src/gui/model/NumberBlockModel.h
  src/gui/model/OrientationBlockModel.cpp
  src/gui/model/OrientationBlockModel.h
  src/gui/model/PathPlannerModelBlockModel.cpp
  src/gui/model/PathPlannerModelBlockModel.h
  src/gui/model/PlotBlockModel.cpp
  src/gui/model/PlotBlockModel.h
  src/gui/model/SliderDockBlockModel.cpp
  src/gui/model/SliderDockBlockModel.h
  src/gui/model/StringBlockModel.cpp
  src/gui/model/StringBlockModel.h
  src/gui/model/TransmissionBlockModel.cpp
  src/gui/model/TransmissionBlockModel.h
  src/gui/model/ValueBlockModel.cpp
  src/gui/model/ValueBlockModel.h
  src/gui/model/VectorBlockModel.cpp
  src/gui/model/VectorBlockModel.h
  src/gui/MyFrameworkWidgetFactory.cpp
  src/gui/MyFrameworkWidgetFactory.h
  src/gui/MyMainWindow.h
  src/gui/NewOpenSaveToolbar.cpp
  src/gui/NewOpenSaveToolbar.h
  src/gui/NewOpenSaveToolbar.ui
  src/gui/PassToolbar.cpp
  src/gui/PassToolbar.h
  src/gui/PassToolbar.ui
  src/gui/SectionControlToolbar.cpp
  src/gui/SectionControlToolbar.h
  src/gui/SettingsDialog.cpp
  src/gui/SettingsDialog.h
  src/gui/SettingsDialog.ui
  src/helpers/anglesHelper.h
  src/helpers/cgalHelper.h
  src/helpers/eigenHelper.h
  src/helpers/GeographicConvertionWrapper.h
  src/helpers/GeoJsonHelper.cpp
  src/helpers/GeoJsonHelper.h
  src/kinematic/cgal.h
  src/kinematic/cgalKernel.h
  src/kinematic/CgalWorker.cpp
  src/kinematic/CgalWorker.h
  src/kinematic/PathPrimitive.cpp
  src/kinematic/PathPrimitive.h
  src/kinematic/PathPrimitiveLine.cpp
  src/kinematic/PathPrimitiveLine.h
  src/kinematic/PathPrimitiveRay.cpp
  src/kinematic/PathPrimitiveRay.h
  src/kinematic/PathPrimitiveSegment.cpp
  src/kinematic/PathPrimitiveSegment.h
  src/kinematic/PathPrimitiveSequence.cpp
  src/kinematic/PathPrimitiveSequence.h
  src/kinematic/Plan.cpp
  src/kinematic/PlanGlobal.cpp
  src/kinematic/PlanGlobal.h
  src/kinematic/Plan.h
  src/kinematic/PoseOptions.h
  src/kinematic/VehicleDynamics/Tire.h
  src/kinematic/VehicleDynamics/TireLinear.h
  src/kinematic/VehicleDynamics/Vehicle.h
  src/kinematic/VehicleDynamics/VehicleNonLinear3DOF.cpp
  src/kinematic/VehicleDynamics/VehicleNonLinear3DOF.h
  src/kinematic/VehicleDynamics/VehicleNonLinear4DOF.cpp
  src/kinematic/VehicleDynamics/VehicleNonLinear4DOF.h
  src/main.cpp
)

set(SOURCES ${SOURCES}
  lib/UBX_Parser/src/UBX_Parser.h)

if(HAVE_SPNAV)
  set(SOURCES ${SOURCES}
    src/gui/SpaceNavigatorPollingThread.cpp
    src/gui/SpaceNavigatorPollingThread.h)
endif()

add_executable(QtOpenGuidance ${SOURCES})

if(HAVE_SPNAV)
  target_compile_definitions(QtOpenGuidance PRIVATE SPNAV_ENABLED)
endif()

target_include_directories(QtOpenGuidance PRIVATE src/)
target_include_directories(QtOpenGuidance PRIVATE src/qnodeseditor/)
target_include_directories(QtOpenGuidance PRIVATE lib/QCustomPlot)
target_include_directories(QtOpenGuidance PRIVATE lib/kalman/include)
target_include_directories(QtOpenGuidance PRIVATE lib/geographiclib/include/)
target_include_directories(QtOpenGuidance PRIVATE lib/UBX_Parser/src/)

target_link_libraries(QtOpenGuidance
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::3DCore
  Qt5::3DExtras
  Qt5::3DRender
  Qt5::3DInput
  Eigen3::Eigen
  KDAB::kddockwidgets
  GeographicLib::GeographicLib
  dubins
  CGAL::CGAL CGAL::CGAL_Core
  QCustomPlot
  qnodeseditor
  ${SPNAV_LIBRARY})

target_precompile_headers(QtOpenGuidance PRIVATE src/pch.h)
