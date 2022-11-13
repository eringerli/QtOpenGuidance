cmake_minimum_required(VERSION 3.16)

project(QtOpenGuidance VERSION 0.9.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include(cmake/modules/UNIFY.cmake)

set(USE_CCACHE true
  CACHE BOOL "use ccache for speeding up repeated builds")

set(USE_DISTCC true
  CACHE BOOL "use distcc for speeding up builds")

set(USE_PCH false
  CACHE BOOL "use pre-compiled headers; speeds up incremental builds for the cost of much disk space")

set(USE_UNITY false
  CACHE BOOL "use unity/jumbo build; speeds up one-time compilation by combining multiple targets into groups")

if(USE_CCACHE)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    message(STATUS "Using ccache")
  else()
    set(CCACHE_PROGRAM "")
    message(STATUS "Not using ccache - enabled but not found")
  endif()
else()
  message(STATUS "Not using ccache")
  set(CCACHE_PROGRAM "")
endif()

if(USE_DISTCC)
  find_program(DISTCC_PROGRAM distcc)
  if(DISTCC_PROGRAM)
    #      set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${DISTCC_PROGRAM}")
    message(STATUS "Using distcc")
  else()
    set(DISTCC_PROGRAM "")
    message(STATUS "Not using distcc - enabled but not found")
  endif()
else()
  message(STATUS "Not using distcc")
  set(DISTCC_PROGRAM "")
endif()

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM} ${DISTCC_PROGRAM}")

if(USE_UNITY)
  message(STATUS "Using Unity/Jumbo build")
  set(CMAKE_UNITY_BUILD true)
else()
  message(STATUS "Not using Unity/Jumbo build")
  set(CMAKE_UNITY_BUILD false)
endif()

if(USE_PCH)
  message(STATUS "Using pre-compiled headers")
else()
  message(STATUS "Not using pre-compiled headers")
endif()

#set(CGAL_DIR lib/CGAL-5.5.1/)
set(Eigen3_DIR lib/eigen)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Concurrent Widgets 3DCore 3DExtras 3DRender 3DInput PrintSupport)
find_package(Qt6 COMPONENTS SerialPort)
#find_package(Qt6Core COMPONENTS PRIVATE_HEADERS)

#find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(KDDockWidgets-qt6 REQUIRED)
find_package(KF5ThreadWeaver REQUIRED)
find_package(CGAL REQUIRED)

find_package(SDL2)

#find_package(CGAL COMPONENTS Core)

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

if (SDL2_FOUND)
  message(STATUS "SDL2 - found")
else()
  message(STATUS "SDL2 - not found")
endif()

set(GEOGRAPHICLIB_LIB_TYPE STATIC)

add_subdirectory(lib/geographiclib/ EXCLUDE_FROM_ALL)
add_subdirectory(lib/dubins_curves/ EXCLUDE_FROM_ALL)

# QCustomPlot
add_library(QCustomPlot
  lib/QCustomPlot/qcustomplot.cpp
  lib/QCustomPlot/qcustomplot.h
)
target_link_libraries(QCustomPlot
  Qt::Core
  Qt::Gui
  Qt::Widgets
  Qt::PrintSupport
)
target_compile_options(QCustomPlot PRIVATE -Wno-deprecated-declarations)

set(SOURCES_qnodeseditor
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
set_source_files_properties(SOURCES_qnodeseditor PROPERTIES UNITY_GROUP "qnodeseditor")

# qnodeseditor
add_library(qnodeseditor "${SOURCES_qnodeseditor}")
target_link_libraries(qnodeseditor
  Qt::Core
  Qt::CorePrivate
  Qt::Gui
  Qt::Widgets
)
target_include_directories(qnodeseditor PRIVATE src/)
target_include_directories(qnodeseditor PRIVATE src/qnodeseditor/)
set(SOURCES_simple
  src/block/base/DebugSink.cpp
  src/block/base/DebugSink.h
  src/block/BlockBase.cpp
  src/block/BlockBase.h
)
addToUnifyGroupAndSources("${SOURCES_simple}" "simpleblocks")


set(SOURCES_filter
  src/filter/3wFRHRL/SystemModel.h
  src/filter/BicycleModel/ImuMeasurementModel.h
  src/filter/BicycleModel/PositionMeasurementModel.h
  src/filter/BicycleModel/SystemModel.h
  src/filter/KinematicModel/ImuMeasurementModel.h
  src/filter/KinematicModel/PositionMeasurementModel.h
  src/filter/KinematicModel/SystemModel.h
  src/filter/MeanMovingWindow.cpp
  src/filter/MeanMovingWindow.h
  src/block/ExtendedKalmanFilter.cpp
  src/block/ExtendedKalmanFilter.h
  src/kinematic/VehicleDynamics/Tire.h
  src/kinematic/VehicleDynamics/TireLinear.cpp
  src/kinematic/VehicleDynamics/TireLinear.h
  src/kinematic/VehicleDynamics/Vehicle.h
  src/kinematic/VehicleDynamics/VehicleNonLinear3DOF.cpp
  src/kinematic/VehicleDynamics/VehicleNonLinear3DOF.h
  src/kinematic/VehicleDynamics/VehicleNonLinear4DOF.cpp
  src/kinematic/VehicleDynamics/VehicleNonLinear4DOF.h
  src/block/global/PoseSimulation.cpp
  src/block/global/PoseSimulation.h
  )
addToUnifyGroupAndSources("${SOURCES_filter}" "filter")

set(SOURCES_gui_toolbar
  src/gui/CameraToolbar.cpp
  src/gui/CameraToolbar.h
  src/gui/FieldsOptimitionToolbar.cpp
  src/gui/FieldsOptimitionToolbar.h
  src/gui/FieldsToolbar.cpp
  src/gui/FieldsToolbar.h
  src/gui/FontComboboxDelegate.h
  src/gui/FontComboboxDelegate.cpp
  src/gui/GlobalPlannerToolbar.cpp
  src/gui/GlobalPlannerToolbar.h
  src/gui/GuidanceTurningToolbar.cpp
  src/gui/GuidanceTurningToolbar.h
  src/gui/GuidanceToolbar.cpp
  src/gui/GuidanceToolbar.h
  src/gui/NewOpenSaveToolbar.cpp
  src/gui/NewOpenSaveToolbar.h
  src/gui/PassToolbar.cpp
  src/gui/PassToolbar.h
  src/gui/SectionControlToolbar.cpp
  src/gui/SectionControlToolbar.h
  )
addToUnifyGroupAndSources("${SOURCES_gui_toolbar}" "gui")

set(SOURCES_gui_windows
  src/main.cpp
  src/gui/SettingsDialog.cpp
  src/gui/SettingsDialog.h
  src/gui/MyFrameworkWidgetFactory.cpp
  src/gui/MyFrameworkWidgetFactory.h
  src/gui/MyMainWindow.cpp
  src/gui/MyMainWindow.h
  )
addToUnifyGroupAndSources("${SOURCES_gui_windows}" "gui_windows")

set(SOURCES_gui_dock
  src/gui/dock/ActionDock.cpp
  src/gui/dock/ActionDock.h
  src/gui/dock/PlotDock.cpp
  src/gui/dock/PlotDock.h
  src/gui/dock/SliderDock.cpp
  src/gui/dock/SliderDock.h
  src/gui/dock/ThreeValuesDock.cpp
  src/gui/dock/ThreeValuesDock.h
  src/gui/dock/ValueDock.cpp
  src/gui/dock/ValueDock.h
  src/gui/dock/XteDock.cpp
  src/gui/dock/XteDock.h
  )
addToUnifyGroupAndSources("${SOURCES_gui_dock}" "gui")

set(SOURCES_model
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
  src/block/graphical/TerrainModel.cpp
  src/block/graphical/TerrainModel.h
  src/gui/model/TransmissionBlockModel.cpp
  src/gui/model/TransmissionBlockModel.h
  src/gui/model/ValueBlockModel.cpp
  src/gui/model/ValueBlockModel.h
  src/gui/model/VectorBlockModel.cpp
  src/gui/model/VectorBlockModel.h
  )
addToUnifyGroupAndSources("${SOURCES_model}" "model")

set(SOURCES_cgal
  src/kinematic/cgal.h
  src/kinematic/cgalKernel.h
  src/kinematic/CgalWorker.cpp
  src/kinematic/CgalWorker.h
  src/helpers/cgalHelper.h
  src/helpers/cgalHelper.cpp
  src/3d/CultivatedAreaMeshGeometry.cpp
  src/3d/CultivatedAreaMeshGeometry.h
  src/block/global/FieldManager.cpp
  src/block/global/FieldManager.h
  )
addToUnifyGroupAndSources("${SOURCES_cgal}" "cgal")

set(SOURCES_3d
  src/3d/ArrowTexture.cpp
  src/3d/ArrowTexture.h
  src/3d/BufferMesh.cpp
  src/3d/BufferMeshGeometry.cpp
  src/3d/BufferMeshGeometry.h
  src/3d/BufferMeshGeometryView.cpp
  src/3d/BufferMeshGeometryView.h
  src/3d/BufferMeshGeometryWithNormals.cpp
  src/3d/BufferMeshGeometryWithNormals.h
  src/3d/BufferMeshGeometryViewWithNormals.cpp
  src/3d/BufferMeshGeometryViewWithNormals.h
  src/3d/BufferMesh.h
  src/3d/BufferMeshWithNormals.cpp
  src/3d/BufferMeshWithNormals.h
  src/3d/CultivatedAreaMesh.cpp
  src/3d/CultivatedAreaMesh.h
  src/3d/CultivatedAreaMeshGeometry.cpp
  src/3d/CultivatedAreaMeshGeometry.h
  src/3d/CultivatedAreaMeshGeometryView.cpp
  src/3d/CultivatedAreaMeshGeometryView.h
  src/3d/texturerendertarget.cpp
  src/3d/texturerendertarget.h
  src/3d/qt3dForwards.h
)
addToUnifyGroupAndSources("${SOURCES_3d}" "3d")

set(SOURCES_arithmetic
  src/block/arithmetic/ArithmeticAddition.cpp
  src/block/arithmetic/ArithmeticAddition.h
  src/block/arithmetic/ArithmeticDivision.cpp
  src/block/arithmetic/ArithmeticDivision.h
  src/block/arithmetic/ArithmeticMultiplication.cpp
  src/block/arithmetic/ArithmeticMultiplication.h
  src/block/arithmetic/ArithmeticSubtraction.cpp
  src/block/arithmetic/ArithmeticSubtraction.h
)
addToUnifyGroupAndSources("${SOURCES_arithmetic}" "simpleblocks")

set(SOURCES_calculation
  src/block/calculation/AckermannSteering.cpp
  src/block/calculation/AckermannSteering.h
  src/block/calculation/AngularVelocityLimiter.cpp
  src/block/calculation/AngularVelocityLimiter.h
  src/block/calculation/TransverseMercatorConverter.cpp
  src/block/calculation/TransverseMercatorConverter.h
)
addToUnifyGroupAndSources("${SOURCES_calculation}" "simpleblocks")

set(SOURCES_comparison
  src/block/comparison/ComparisonEqualTo.cpp
  src/block/comparison/ComparisonEqualTo.h
  src/block/comparison/ComparisonGreaterOrEqualTo.cpp
  src/block/comparison/ComparisonGreaterOrEqualTo.h
  src/block/comparison/ComparisonGreaterThan.cpp
  src/block/comparison/ComparisonGreaterThan.h
  src/block/comparison/ComparisonLessOrEqualTo.cpp
  src/block/comparison/ComparisonLessOrEqualTo.h
  src/block/comparison/ComparisonLessThan.cpp
  src/block/comparison/ComparisonLessThan.h
  src/block/comparison/ComparisonNotEqualTo.cpp
  src/block/comparison/ComparisonNotEqualTo.h
)
addToUnifyGroupAndSources("${SOURCES_comparison}" "simpleblocks")

set(SOURCES_converter
  src/block/converter/CommunicationJrk.cpp
  src/block/converter/CommunicationJrk.h
  src/block/converter/CommunicationPgn7FFE.cpp
  src/block/converter/CommunicationPgn7FFE.h
  src/block/converter/ValueTransmissionBase64Data.cpp
  src/block/converter/ValueTransmissionBase64Data.h
  src/block/converter/ValueTransmissionBase.cpp
  src/block/converter/ValueTransmissionBase.h
  src/block/converter/ValueTransmissionNumber.cpp
  src/block/converter/ValueTransmissionNumber.h
  src/block/converter/ValueTransmissionQuaternion.cpp
  src/block/converter/ValueTransmissionQuaternion.h
  src/block/converter/ValueTransmissionState.cpp
  src/block/converter/ValueTransmissionState.h
)
addToUnifyGroupAndSources("${SOURCES_converter}" "simpleblocks")

set(SOURCES_dock_display
  src/block/dock/display/OrientationDockBlock.cpp
  src/block/dock/display/OrientationDockBlock.h
  src/block/dock/display/PositionDockBlock.cpp
  src/block/dock/display/PositionDockBlock.h
  src/block/dock/display/ValueDockBlockBase.cpp
  src/block/dock/display/ValueDockBlockBase.h
  src/block/dock/display/ValueDockBlock.cpp
  src/block/dock/display/ValueDockBlock.h
  src/block/dock/display/XteDockBlock.cpp
  src/block/dock/display/XteDockBlock.h
)
addToUnifyGroupAndSources("${SOURCES_dock_display}" "dock")

set(SOURCES_dock_input
  src/block/dock/input/ActionDockBlock.cpp
  src/block/dock/input/ActionDockBlock.h
  src/block/dock/input/SliderDockBlock.cpp
  src/block/dock/input/SliderDockBlock.h
  )
addToUnifyGroupAndSources("${SOURCES_dock_input}" "dock")

set(SOURCES_dock_plot
  src/block/dock/plot/OrientationPlotDockBlock.cpp
  src/block/dock/plot/OrientationPlotDockBlock.h
  src/block/dock/plot/PlotDockBlockBase.cpp
  src/block/dock/plot/PlotDockBlockBase.h
  src/block/dock/plot/ValuePlotDockBlock.cpp
  src/block/dock/plot/ValuePlotDockBlock.h
)
addToUnifyGroupAndSources("${SOURCES_dock_plot}" "dock")

set(SOURCES_global
  src/block/global/CameraController.cpp
  src/block/global/CameraController.h
  src/block/global/FpsMeasurement.cpp
  src/block/global/FpsMeasurement.h
  src/block/global/GridModel.cpp
  src/block/global/GridModel.h
)
addToUnifyGroupAndSources("${SOURCES_global}" "global")

set(SOURCES_graphical
  src/block/graphical/CultivatedAreaModel.cpp
  src/block/graphical/CultivatedAreaModel.h
  src/block/graphical/GlobalPlannerModel.cpp
  src/block/graphical/GlobalPlannerModel.h
  src/block/graphical/PathPlannerModel.cpp
  src/block/graphical/PathPlannerModel.h
  src/block/graphical/SprayerModel.cpp
  src/block/graphical/SprayerModel.h
  src/block/graphical/TractorModel.cpp
  src/block/graphical/TractorModel.h
  src/block/graphical/TrailerModel.cpp
  src/block/graphical/TrailerModel.h
)
addToUnifyGroupAndSources("${SOURCES_graphical}" "graphical")

set(SOURCES_guidance
  src/block/guidance/LocalPlanner.cpp
  src/block/guidance/LocalPlanner.h
  src/block/guidance/GlobalPlanner.cpp
  src/block/guidance/GlobalPlanner.h
  src/block/guidance/PoseSynchroniser.cpp
  src/block/guidance/PoseSynchroniser.h
  src/block/guidance/StanleyGuidance.cpp
  src/block/guidance/StanleyGuidance.h
  src/block/guidance/XteGuidance.cpp
  src/block/guidance/XteGuidance.h
)
addToUnifyGroupAndSources("${SOURCES_guidance}" "guidance")

set(SOURCES_kinematic
  src/block/kinematic/FixedKinematic.cpp
  src/block/kinematic/FixedKinematic.h
  src/block/kinematic/FixedKinematicPrimitive.cpp
  src/block/kinematic/FixedKinematicPrimitive.h
  src/block/kinematic/TrailerKinematic.cpp
  src/block/kinematic/TrailerKinematic.h
  src/block/kinematic/TrailerKinematicPrimitive.cpp
  src/block/kinematic/TrailerKinematicPrimitive.h
)
addToUnifyGroupAndSources("${SOURCES_kinematic}" "simpleblocks2")

set(SOURCES_literal
  src/block/literal/NumberObject.cpp
  src/block/literal/NumberObject.h
  src/block/literal/OrientationBlock.cpp
  src/block/literal/OrientationBlock.h
  src/block/literal/StringObject.cpp
  src/block/literal/StringObject.h
  src/block/literal/VectorObject.cpp
  src/block/literal/VectorObject.h
  )
addToUnifyGroupAndSources("${SOURCES_literal}" "simpleblocks2")

set(SOURCES_parser
  src/block/parser/NmeaParserGGA.cpp
  src/block/parser/NmeaParserGGA.h
  src/block/parser/NmeaParserHDT.cpp
  src/block/parser/NmeaParserHDT.h
  src/block/parser/NmeaParserRMC.cpp
  src/block/parser/NmeaParserRMC.h
  src/block/parser/UbxParser.cpp
  src/block/parser/UbxParser.h
  )
addToUnifyGroupAndSources("${SOURCES_parser}" "simpleblocks2")

set(SOURCES_sectionControl
  src/block/sectionControl/Implement.cpp
  src/block/sectionControl/Implement.h
  src/block/sectionControl/ImplementSection.h
  src/block/sectionControl/SectionControl.cpp
  src/block/sectionControl/SectionControl.h
  )
addToUnifyGroupAndSources("${SOURCES_sectionControl}" "simpleblocks2")


set(SOURCES_stream
  src/block/stream/FileStream.cpp
  src/block/stream/FileStream.h
  src/block/stream/UdpSocket.cpp
  src/block/stream/UdpSocket.h
  )
addToUnifyGroupAndSources("${SOURCES_stream}" "stream")

if(Qt6SerialPort_DIR)
  set(SOURCES_serialport
    src/block/stream/SerialPort.cpp
    src/block/stream/SerialPort.h
    )
  addToUnifyGroupAndSources("${SOURCES_serialport}" "stream")

  set(QTSERIAL_LIBRARY "Qt::SerialPort")
endif()

set(SOURCES_helpers
  src/helpers/anglesHelper.h
  src/helpers/eigenHelper.h
  src/helpers/eigenHelper.cpp
  src/helpers/GeographicConvertionWrapper.cpp
  src/helpers/GeographicConvertionWrapper.h
  src/helpers/GeoJsonHelper.cpp
  src/helpers/GeoJsonHelper.h
  )
addToUnifyGroupAndSources("${SOURCES_helpers}" "helpers")

set(SOURCES_kinematic
  src/kinematic/CalculationOptions.h
  src/kinematic/PathPrimitive.cpp
  src/kinematic/PathPrimitive.h
  src/kinematic/PathPrimitiveArc.cpp
  src/kinematic/PathPrimitiveArc.h
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
  )
addToUnifyGroupAndSources("${SOURCES_kinematic}" "kinematic")

set(SOURCES_job
  src/job/ConnectPointsJob.cpp
  src/job/ConnectPointsJob.h
  src/job/CultivatedAreaOptimitionController.cpp
  src/job/CultivatedAreaOptimitionController.h
  src/job/FieldOptimitionController.cpp
  src/job/FieldOptimitionController.h
  src/job/PlanOptimitionController.cpp
  src/job/PlanOptimitionController.h
  src/job/TestCollinearityJob.cpp
  src/job/TestCollinearityJob.h
)
addToUnifyGroupAndSources("${SOURCES_job}" "job")

set(SOURCES_qnodeseditor
  src/qnodeseditor/qneblock.cpp
  src/qnodeseditor/qneblock.h
  src/qnodeseditor/qneconnection.cpp
  src/qnodeseditor/qneconnection.h
  src/qnodeseditor/qnegestures.cpp
  src/qnodeseditor/qnegestures.h
  src/qnodeseditor/qnegraphicsview.cpp
  src/qnodeseditor/qnegraphicsview.h
  src/qnodeseditor/qneport.cpp
  src/qnodeseditor/qneport.h
  src/qnodeseditor/qnodeseditor.cpp
  src/qnodeseditor/qnodeseditor.h
  )
addToUnifyGroupAndSources("${SOURCES_qnodeseditor}" "qnodeseditor")

set(SOURCES ${SOURCES}
  lib/UBX_Parser/src/UBX_Parser.h)

if(HAVE_SPNAV)
  set(SOURCES_spnav
    src/gui/SpaceNavigatorPollingThread.cpp
    src/gui/SpaceNavigatorPollingThread.h)
  addToUnifyGroupAndSources("${SOURCES_spnav}" "gui_windows")
endif()

if (SDL2_FOUND)
  set(SOURCES_sdl2
    src/gui/SdlInputPollingThread.cpp
    src/gui/SdlInputPollingThread.h)
  addToUnifyGroupAndSources("${SOURCES_sdl2}" "gui_windows")
endif()

add_executable(QtOpenGuidance ${SOURCES})

set_target_properties(QtOpenGuidance PROPERTIES
                      UNITY_BUILD_MODE GROUP
                      )

if(HAVE_SPNAV)
  target_compile_definitions(QtOpenGuidance PRIVATE SPNAV_ENABLED)
endif()

if(SDL2_FOUND)
  target_compile_definitions(QtOpenGuidance PRIVATE SDL2_ENABLED)
endif()

if(Qt6SerialPort_DIR)
  target_compile_definitions(QtOpenGuidance PRIVATE SERIALPORT_ENABLED)
endif()

target_include_directories(QtOpenGuidance PRIVATE src/)
target_include_directories(QtOpenGuidance PRIVATE src/qnodeseditor/)
target_include_directories(QtOpenGuidance PRIVATE lib/QCustomPlot)
target_include_directories(QtOpenGuidance PRIVATE lib/kalman/include)
target_include_directories(QtOpenGuidance PRIVATE lib/geographiclib/include/)
target_include_directories(QtOpenGuidance PRIVATE lib/UBX_Parser/src/)

target_include_directories(QtOpenGuidance PRIVATE lib/eigen)

target_link_libraries(QtOpenGuidance
  Qt::Core
  Qt::CorePrivate
  Qt::Gui
  Qt::Widgets
  Qt::3DCore
  Qt::3DExtras
  Qt::3DRender
  Qt::3DInput
  ${QTSERIAL_LIBRARY}
  KF5::ThreadWeaver
#  Eigen3::Eigen
  KDAB::kddockwidgets
  GeographicLib::GeographicLib_STATIC
  dubins
  CGAL::CGAL # CGAL::CGAL_Core
  QCustomPlot
  qnodeseditor
  ${SPNAV_LIBRARY}
  ${SDL2_LIBRARIES}
)

if(USE_PCH)
  target_precompile_headers(QtOpenGuidance PRIVATE src/pch.h)
endif()

install(TARGETS QtOpenGuidance)

file(GLOB CONFIG_FILES ${PROJECT_SOURCE_DIR}/config/*.json)
install(FILES ${CONFIG_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/QtOpenGuidance/config)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/icons/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/)

install(FILES QtOpenGuidance.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
