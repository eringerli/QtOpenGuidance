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

CONFIG += c++14 ccache

#unix {
#    CONFIG += precompile_header
#}

include(build-paths.pri)

# enable to recheck all compile tests/find the libraries
# CONFIG += recheck

QT += 3dcore 3drender 3dinput 3dlogic 3dextras concurrent
QT += widgets core

qtCompileTest(serialport) {
    QT += serialport
    DEFINES += SERIALPORT_ENABLED
    HEADERS += src/block/SerialPort.h
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
    src/block/ActionDockBlock.cpp \
    src/block/FieldManager.cpp \
    src/block/GlobalPlanner.cpp \
    src/block/LocalPlanner.cpp \
    src/block/PathPlannerModel.cpp \
    src/block/SliderDockBlock.cpp \
    src/block/SprayerModel.cpp \
    src/block/ValueDockBlockBase.cpp \
    src/block/ValueTransmissionBase.cpp \
    src/block/XteDockBlock.cpp \
    src/gui/ActionDockBlockModel.cpp \
    src/gui/ActionToolbar.cpp \
    src/gui/FieldsOptimitionToolbar.cpp \
    src/gui/FieldsToolbar.cpp \
    src/gui/GlobalPlannerToolbar.cpp \
    src/gui/MyFrameworkWidgetFactory.cpp \
    src/gui/NewOpenSaveToolbar.cpp \
    src/gui/PathPlannerModelBlockModel.cpp \
    src/gui/SectionControlToolbar.cpp \
    src/gui/SliderDock.cpp \
    src/gui/SliderDockBlockModel.cpp \
    src/gui/ThreeValuesDock.cpp \
    src/gui/TransmissionBlockModel.cpp \
    src/gui/ValueBlockModel.cpp \
    src/gui/ValueDock.cpp \
    src/gui/XteDock.cpp \
    src/kinematic/CgalWorker.cpp \
    src/kinematic/PathPrimitive.cpp \
    src/kinematic/PathPrimitiveLine.cpp \
    src/kinematic/PathPrimitiveRay.cpp \
    src/kinematic/PathPrimitiveSegment.cpp \
    src/kinematic/PathPrimitiveSequence.cpp \
    src/kinematic/Plan.cpp \
    src/kinematic/PlanGlobal.cpp \
    src/main.cpp \
    src/block/PoseSimulation.cpp \
    src/block/TractorModel.cpp \
    src/block/TrailerModel.cpp \
    src/gui/CameraToolbar.cpp \
    src/gui/GuidanceToolbar.cpp \
    src/gui/GuidanceTurning.cpp \
    src/gui/ImplementBlockModel.cpp \
    src/gui/ImplementSectionModel.cpp \
    src/gui/NumberBlockModel.cpp \
    src/gui/PassToolbar.cpp \
    src/gui/SettingsDialog.cpp \
    src/gui/StringBlockModel.cpp \
    src/gui/VectorBlockModel.cpp

HEADERS += \
    src/3d/ArrowTexture.h \
    src/3d/BufferMesh.h \
    src/3d/BufferMeshGeometry.h \
    src/block/AckermannSteering.h \
    src/block/ActionDockBlock.h \
    src/block/AngularVelocityLimiter.h \
    src/block/BlockBase.h \
    src/block/CameraController.h \
    src/block/CommunicationJrk.h \
    src/block/CommunicationPgn7FFE.h \
    src/block/DebugSink.h \
    src/block/FieldManager.h \
    src/block/FileStream.h \
    src/block/FpsMeasurement.h \
    src/block/GlobalPlanner.h \
    src/block/GridModel.h \
    src/block/Implement.h \
    src/block/ImplementSection.h \
    src/block/LocalPlanner.h \
    src/block/NmeaParserGGA.h \
    src/block/NmeaParserHDT.h \
    src/block/NmeaParserRMC.h \
    src/block/NumberObject.h \
    src/block/OrientationDockBlock.h \
    src/block/PathPlannerModel.h \
    src/block/PoseSimulation.h \
    src/block/PoseSynchroniser.h \
    src/block/PositionDockBlock.h \
    src/block/SliderDockBlock.h \
    src/block/SprayerModel.h \
    src/block/StanleyGuidance.h \
    src/block/StringObject.h \
    src/block/TractorModel.h \
    src/block/TrailerModel.h \
    src/block/TransverseMercatorConverter.h \
    src/block/UbxParser.h \
    src/block/UdpSocket.h \
    src/block/ValueDockBlock.h \
    src/block/ValueDockBlockBase.h \
    src/block/ValueTransmissionBase.h \
    src/block/ValueTransmissionBase64Data.h \
    src/block/ValueTransmissionNumber.h \
    src/block/ValueTransmissionQuaternion.h \
    src/block/ValueTransmissionState.h \
    src/block/VectorObject.h \
    src/block/XteDockBlock.h \
    src/block/XteGuidance.h \
    src/gui/ActionDockBlockModel.h \
    src/gui/ActionToolbar.h \
    src/gui/CameraToolbar.h \
    src/gui/FieldsOptimitionToolbar.h \
    src/gui/FieldsToolbar.h \
    src/gui/FontComboboxDelegate.h \
    src/gui/GlobalPlannerToolbar.h \
    src/gui/GuidanceToolbar.h \
    src/gui/GuidanceTurning.h \
    src/gui/ImplementBlockModel.h \
    src/gui/ImplementSectionModel.h \
    src/gui/MyFrameworkWidgetFactory.h \
    src/gui/MyMainWindow.h \
    src/gui/NewOpenSaveToolbar.h \
    src/gui/NumberBlockModel.h \
    src/gui/PassToolbar.h \
    src/gui/PathPlannerModelBlockModel.h \
    src/gui/SectionControlToolbar.h \
    src/gui/SettingsDialog.h \
    src/gui/SliderDock.h \
    src/gui/SliderDockBlockModel.h \
    src/gui/StringBlockModel.h \
    src/gui/ThreeValuesDock.h \
    src/gui/TransmissionBlockModel.h \
    src/gui/ValueBlockModel.h \
    src/gui/ValueDock.h \
    src/gui/VectorBlockModel.h \
    src/gui/XteDock.h \
    src/kinematic/CgalWorker.h \
    src/kinematic/FixedKinematic.h \
    src/kinematic/GeographicConvertionWrapper.h \
    src/kinematic/PathPrimitive.h \
    src/kinematic/PathPrimitiveLine.h \
    src/kinematic/PathPrimitiveRay.h \
    src/kinematic/PathPrimitiveSegment.h \
    src/kinematic/PathPrimitiveSequence.h \
    src/kinematic/Plan.h \
    src/kinematic/PlanGlobal.h \
    src/kinematic/PoseOptions.h \
    src/kinematic/TrailerKinematic.h

unix {
    PRECOMPILED_HEADER  = src/pch.h
}

HEADERS += \
    src/cgal.h \
    src/cgalKernel.h

FORMS += \
    src/gui/ActionToolbar.ui \
    src/gui/CameraToolbar.ui \
    src/gui/FieldsOptimitionToolbar.ui \
    src/gui/FieldsToolbar.ui \
    src/gui/GlobalPlannerToolbar.ui \
    src/gui/GuidanceToolbar.ui \
    src/gui/GuidanceTurning.ui \
    src/gui/NewOpenSaveToolbar.ui \
    src/gui/PassToolbar.ui \
    src/gui/SettingsDialog.ui \
    src/gui/SliderDock.ui \
    src/gui/ThreeValuesDock.ui \
    src/gui/ValueDock.ui \
    src/gui/XteDock.ui

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
