# Copyright( C ) 2020 Christian Riggenbach
#
# This program is free software:
# you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# ( at your option ) any later version.
#
# This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY;
# without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

load(configure)

CONFIG += c++17 ccache

unix {
    CONFIG += precompile_header
    QMAKE_CXXFLAGS += -Wno-ignored-attributes
}

include(build-paths.pri)

# enable to recheck all compile tests/find the libraries
# CONFIG += recheck

QT += 3dcore 3drender 3dinput 3dlogic 3dextras concurrent
QT += widgets core

DEFINES += _USE_MATH_DEFINES

qtCompileTest(serialport) {
    QT += serialport
    DEFINES += SERIALPORT_ENABLED
    HEADERS += \
    src/block/stream/SerialPort.h
    CONFIG += precompile_header
}

qtCompileTest(spnav) {
    DEFINES += SPNAV_ENABLED
    HEADERS += src/gui/SpaceNavigatorPollingThread.h
    LIBS += -lspnav
}

SOURCES += \
    src/3d/BufferMesh.cpp \
    src/3d/BufferMeshGeometry.cpp \
    src/3d/CultivatedAreaMesh.cpp \
    src/3d/CultivatedAreaMeshGeometry.cpp \
    src/3d/texturerendertarget.cpp \
    src/block/BlockBase.cpp \
    src/block/converter/ValueTransmissionBase.cpp \
    src/block/dock/display/ValueDockBlockBase.cpp \
    src/block/dock/display/XteDockBlock.cpp \
    src/block/dock/input/ActionDockBlock.cpp \
    src/block/dock/input/SliderDockBlock.cpp \
    src/block/dock/plot/PlotDockBlockBase.cpp \
    src/block/global/FieldManager.cpp \
    src/block/global/GlobalPlanner.cpp \
    src/block/global/PoseSimulation.cpp \
    src/block/graphical/CultivatedAreaModel.cpp \
    src/block/graphical/PathPlannerModel.cpp \
    src/block/graphical/SprayerModel.cpp \
    src/block/graphical/TractorModel.cpp \
    src/block/graphical/TrailerModel.cpp \
    src/block/guidance/LocalPlanner.cpp \
    src/block/guidance/StanleyGuidance.cpp \
    src/block/sectionControl/SectionControl.cpp \
    src/gui/FieldsOptimitionToolbar.cpp \
    src/gui/FieldsToolbar.cpp \
    src/gui/GlobalPlannerToolbar.cpp \
    src/gui/MyFrameworkWidgetFactory.cpp \
    src/gui/NewOpenSaveToolbar.cpp \
    src/gui/SectionControlToolbar.cpp \
    src/gui/dock/ActionDock.cpp \
    src/gui/dock/PlotDock.cpp \
    src/gui/dock/SliderDock.cpp \
    src/gui/dock/ThreeValuesDock.cpp \
    src/gui/dock/ValueDock.cpp \
    src/gui/dock/XteDock.cpp \
    src/gui/model/ActionDockBlockModel.cpp \
    src/gui/model/ImplementBlockModel.cpp \
    src/gui/model/ImplementSectionModel.cpp \
    src/gui/model/NumberBlockModel.cpp \
    src/gui/model/OrientationBlockModel.cpp \
    src/gui/model/PathPlannerModelBlockModel.cpp \
    src/gui/model/PlotBlockModel.cpp \
    src/gui/model/SliderDockBlockModel.cpp \
    src/gui/model/StringBlockModel.cpp \
    src/gui/model/TransmissionBlockModel.cpp \
    src/gui/model/ValueBlockModel.cpp \
    src/gui/model/VectorBlockModel.cpp \
    src/helpers/GeoJsonHelper.cpp \
    src/kinematic/CgalWorker.cpp \
    src/kinematic/PathPrimitive.cpp \
    src/kinematic/PathPrimitiveLine.cpp \
    src/kinematic/PathPrimitiveRay.cpp \
    src/kinematic/PathPrimitiveSegment.cpp \
    src/kinematic/PathPrimitiveSequence.cpp \
    src/kinematic/Plan.cpp \
    src/kinematic/PlanGlobal.cpp \
    src/main.cpp \
    src/gui/CameraToolbar.cpp \
    src/gui/GuidanceToolbar.cpp \
    src/gui/GuidanceTurning.cpp \
    src/gui/PassToolbar.cpp \
    src/gui/SettingsDialog.cpp

HEADERS += \
    src/3d/ArrowTexture.h \
    src/3d/BufferMesh.h \
    src/3d/BufferMeshGeometry.h \
    src/3d/CultivatedAreaMesh.h \
    src/3d/CultivatedAreaMeshGeometry.h \
    src/3d/texturerendertarget.h \
    src/block/BlockBase.h \
    src/block/ExtendedKalmanFilter.h \
    src/block/arithmetic/ArithmeticAddition.h \
    src/block/arithmetic/ArithmeticDivision.h \
    src/block/arithmetic/ArithmeticMultiplication.h \
    src/block/arithmetic/ArithmeticSubtraction.h \
    src/block/base/DebugSink.h \
    src/block/calculation/AckermannSteering.h \
    src/block/calculation/AngularVelocityLimiter.h \
    src/block/calculation/TransverseMercatorConverter.h \
    src/block/comparison/ComparisonEqualTo.h \
    src/block/comparison/ComparisonGreaterOrEqualTo.h \
    src/block/comparison/ComparisonGreaterThan.h \
    src/block/comparison/ComparisonLessOrEqualTo.h \
    src/block/comparison/ComparisonLessThan.h \
    src/block/comparison/ComparisonNotEqualTo.h \
    src/block/converter/CommunicationJrk.h \
    src/block/converter/CommunicationPgn7FFE.h \
    src/block/converter/ValueTransmissionBase.h \
    src/block/converter/ValueTransmissionBase64Data.h \
    src/block/converter/ValueTransmissionNumber.h \
    src/block/converter/ValueTransmissionQuaternion.h \
    src/block/converter/ValueTransmissionState.h \
    src/block/dock/display/OrientationDockBlock.h \
    src/block/dock/display/PositionDockBlock.h \
    src/block/dock/display/ValueDockBlock.h \
    src/block/dock/display/ValueDockBlockBase.h \
    src/block/dock/display/XteDockBlock.h \
    src/block/dock/input/ActionDockBlock.h \
    src/block/dock/input/SliderDockBlock.h \
    src/block/dock/plot/OrientationPlotDockBlock.h \
    src/block/dock/plot/PlotDockBlockBase.h \
    src/block/dock/plot/ValuePlotDockBlock.h \
    src/block/global/CameraController.h \
    src/block/global/FieldManager.h \
    src/block/global/FpsMeasurement.h \
    src/block/global/GlobalPlanner.h \
    src/block/global/GridModel.h \
    src/block/global/PoseSimulation.h \
    src/block/graphical/CultivatedAreaModel.h \
    src/block/graphical/PathPlannerModel.h \
    src/block/graphical/SprayerModel.h \
    src/block/graphical/TractorModel.h \
    src/block/graphical/TrailerModel.h \
    src/block/guidance/LocalPlanner.h \
    src/block/guidance/PoseSynchroniser.h \
    src/block/guidance/StanleyGuidance.h \
    src/block/guidance/XteGuidance.h \
    src/block/literal/NumberObject.h \
    src/block/literal/OrientationBlock.h \
    src/block/literal/StringObject.h \
    src/block/literal/VectorObject.h \
    src/block/parser/NmeaParserGGA.h \
    src/block/parser/NmeaParserHDT.h \
    src/block/parser/NmeaParserRMC.h \
    src/block/parser/UbxParser.h \
    src/block/sectionControl/Implement.h \
    src/block/sectionControl/ImplementSection.h \
    src/block/sectionControl/SectionControl.h \
    src/block/stream/FileStream.h \
    src/block/stream/UdpSocket.h \
    src/filter/3wFRHRL/SystemModel.h \
    src/filter/BicycleModel/ImuMeasurementModel.h \
    src/filter/BicycleModel/PositionMeasurementModel.h \
    src/filter/BicycleModel/SystemModel.h \
    src/filter/KinematicModel/ImuMeasurementModel.h \
    src/filter/KinematicModel/PositionMeasurementModel.h \
    src/filter/KinematicModel/SystemModel.h \
    src/filter/MeanMovingWindow.h \
    src/gui/CameraToolbar.h \
    src/gui/FieldsOptimitionToolbar.h \
    src/gui/FieldsToolbar.h \
    src/gui/FontComboboxDelegate.h \
    src/gui/GlobalPlannerToolbar.h \
    src/gui/GuidanceToolbar.h \
    src/gui/GuidanceTurning.h \
    src/gui/MyFrameworkWidgetFactory.h \
    src/gui/MyMainWindow.h \
    src/gui/NewOpenSaveToolbar.h \
    src/gui/PassToolbar.h \
    src/gui/SectionControlToolbar.h \
    src/gui/SettingsDialog.h \
    src/gui/dock/ActionDock.h \
    src/gui/dock/PlotDock.h \
    src/gui/dock/SliderDock.h \
    src/gui/dock/ThreeValuesDock.h \
    src/gui/dock/ValueDock.h \
    src/gui/dock/XteDock.h \
    src/gui/model/ActionDockBlockModel.h \
    src/gui/model/ImplementBlockModel.h \
    src/gui/model/ImplementSectionModel.h \
    src/gui/model/NumberBlockModel.h \
    src/gui/model/OrientationBlockModel.h \
    src/gui/model/PathPlannerModelBlockModel.h \
    src/gui/model/PlotBlockModel.h \
    src/gui/model/SliderDockBlockModel.h \
    src/gui/model/StringBlockModel.h \
    src/gui/model/TransmissionBlockModel.h \
    src/gui/model/ValueBlockModel.h \
    src/gui/model/VectorBlockModel.h \
    src/helpers/GeoJsonHelper.h \
    src/helpers/GeographicConvertionWrapper.h \
    src/helpers/cgalHelper.h \
    src/helpers/eigenHelper.h \
    src/helpers/normalizeAngles.h \
    src/kinematic/CgalWorker.h \
    src/kinematic/FixedKinematic.h \
    src/kinematic/FixedKinematicPrimitive.h \
    src/kinematic/PathPrimitive.h \
    src/kinematic/PathPrimitiveLine.h \
    src/kinematic/PathPrimitiveRay.h \
    src/kinematic/PathPrimitiveSegment.h \
    src/kinematic/PathPrimitiveSequence.h \
    src/kinematic/Plan.h \
    src/kinematic/PlanGlobal.h \
    src/kinematic/PoseOptions.h \
    src/kinematic/TrailerKinematic.h \
    src/kinematic/TrailerKinematicPrimitive.h \

unix {
    PRECOMPILED_HEADER  = src/pch.h
}

HEADERS += \
    src/kinematic/cgal.h \
    src/kinematic/cgalKernel.h

FORMS += \
    src/gui/CameraToolbar.ui \
    src/gui/FieldsOptimitionToolbar.ui \
    src/gui/FieldsToolbar.ui \
    src/gui/GlobalPlannerToolbar.ui \
    src/gui/GuidanceToolbar.ui \
    src/gui/GuidanceTurning.ui \
    src/gui/NewOpenSaveToolbar.ui \
    src/gui/PassToolbar.ui \
    src/gui/SettingsDialog.ui \
    src/gui/dock/ActionDock.ui \
    src/gui/dock/PlotDock.ui \
    src/gui/dock/SliderDock.ui \
    src/gui/dock/ThreeValuesDock.ui \
    src/gui/dock/ValueDock.ui \
    src/gui/dock/XteDock.ui

RESOURCES += \
    res/ressources.qrc

!unix | android {
    RESOURCES += \
        lib/oxygen-icons-png/oxygen.qrc
}

include($$PWD/src/qnodeseditor/qnodeeditor.pri)
include($$PWD/lib/geographiclib.pri)
include($$PWD/lib/cgal.pri)
include($$PWD/lib/UBX_Parser.pri)
include($$PWD/lib/dubins_curves.pri)
include($$PWD/lib/eigen.pri)
include($$PWD/lib/kalman.pri)
include($$PWD/lib/qcustomplot.pri)

# KDDockWidgets
INCLUDEPATH += $$KDDOCKWIDGET_INCLUDE
LIBS += -L$$KDDOCKWIDGET_LIBPATH -l$$KDDOCKWIDGET_LIBRARY

android {
    ANDROID_EXTRA_LIBS += $${KDDOCKWIDGET_LIBPATH}/lib$${KDDOCKWIDGET_LIBRARY}.so
}

android {
    QT += androidextras
    configs.path = $$DATADIR/configs
}

isEmpty(PREFIX) {
  PREFIX = /usr
}

target.path = $$PREFIX/bin
INSTALLS += target

configs.files = config/*
configs.path = $$PREFIX/share/QtOpenGuidance/config/
INSTALLS += configs

icons.files = icons/*
icons.path = $$PREFIX/share/icons/hicolor/
INSTALLS += icons

desktop-entry.files = QtOpenGuidance.desktop
desktop-entry.path = $$PREFIX/share/applications/
INSTALLS += desktop-entry

DISTFILES += PKGBUILD QtOpenGuidance.desktop README.md

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

SOURCES += \
    src/3d/3d-moc.cpp \
    src/block/block-moc.cpp \
    src/gui/gui-moc.cpp \
    src/kinematic/kinematic-moc.cpp
