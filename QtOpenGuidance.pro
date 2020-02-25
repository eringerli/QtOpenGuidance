load(configure)

CONFIG += c++14 ccache

# enable to recheck all compile tests/find the libraries
# CONFIG += recheck

QT += 3dcore 3drender 3dinput 3dlogic 3dextras concurrent
QT += widgets core

!android {
    QT += serialport
    DEFINES += SERIALPORT_ENABLED
    HEADERS += src/block/SerialPort.h
    CONFIG += precompile_header
}

android {
    INCLUDEPATH += android/arm64-v8a//include/boost-1_70/
    LIBS += -Landroid/arm64-v8a/lib/
    QMAKE_CXXFLAGS += -fexceptions
    QMAKE_CXXFLAGS += -frtti
}

qtCompileTest(spnav) {
    DEFINES += SPNAV_ENABLED
    HEADERS += src/gui/SpaceNavigatorPollingThread.h
    LIBS += -lspnav
}

SOURCES += \
    src/3d/BufferMesh.cpp \
    src/3d/BufferMeshGeometry.cpp \
    src/block/FieldManager.cpp \
    src/block/SprayerModel.cpp \
    src/block/ValueDockBlockBase.cpp \
    src/gui/FieldsOptimitionToolbar.cpp \
    src/gui/FieldsToolbar.cpp \
    src/gui/MyFrameworkWidgetFactory.cpp \
    src/gui/SectionControlToolbar.cpp \
    src/gui/ThreeValuesDock.cpp \
    src/gui/ValueBlockModel.cpp \
    src/gui/ValueDock.cpp \
    src/gui/XteDock.cpp \
    src/kinematic/CgalWorker.cpp \
    src/kinematic/PathPrimitive.cpp \
    src/main.cpp \
    src/block/GuidanceGlobalPlanner.cpp \
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
    src/gui/SimulatorToolbar.cpp \
    src/gui/StringBlockModel.cpp \
    src/gui/VectorBlockModel.cpp
    
HEADERS += \
    src/3d/ArrowTexture.h \
    src/3d/BufferMesh.h \
    src/3d/BufferMeshGeometry.h \
    src/block/AckermannSteering.h \
    src/block/BlockBase.h \
    src/block/CameraController.h \
    src/block/CommunicationJrk.h \
    src/block/CommunicationPgn7FFE.h \
    src/block/DebugSink.h \
    src/block/FieldManager.h \
    src/block/FileStream.h \
    src/block/FpsMeasurement.h \
    src/block/GridModel.h \
    src/block/GuidanceGlobalPlanner.h \
    src/block/GuidanceGlobalPlannerModel.h \
    src/block/GuidanceLocalPlanner.h \
    src/block/GuidancePlannerGui.h \
    src/block/GuidanceStanley.h \
    src/block/GuidanceXte.h \
    src/block/Implement.h \
    src/block/ImplementSection.h \
    src/block/NmeaParserGGA.h \
    src/block/NmeaParserHDT.h \
    src/block/NmeaParserRMC.h \
    src/block/NumberObject.h \
    src/block/OrientationDockBlock.h \
    src/block/PoseSimulation.h \
    src/block/PoseSynchroniser.h \
    src/block/PositionDockBlock.h \
    src/block/PrintLatency.h \
    src/block/SprayerModel.h \
    src/block/StringObject.h \
    src/block/TractorModel.h \
    src/block/TrailerModel.h \
    src/block/TransverseMercatorConverter.h \
    src/block/UdpSocket.h \
    src/block/ValueDockBlock.h \
    src/block/ValueDockBlockBase.h \
    src/block/VectorObject.h \
    src/block/XteDockBlock.h \
    src/gui/CameraToolbar.h \
    src/gui/FieldsOptimitionToolbar.h \
    src/gui/FieldsToolbar.h \
    src/gui/FontComboboxDelegate.h \
    src/gui/GuidanceToolbar.h \
    src/gui/GuidanceTurning.h \
    src/gui/ImplementBlockModel.h \
    src/gui/ImplementSectionModel.h \
    src/gui/MyFrameworkWidgetFactory.h \
    src/gui/MyMainWindow.h \
    src/gui/NumberBlockModel.h \
    src/gui/PassToolbar.h \
    src/gui/SectionControlToolbar.h \
    src/gui/SettingsDialog.h \
    src/gui/SimulatorToolbar.h \
    src/gui/StringBlockModel.h \
    src/gui/ThreeValuesDock.h \
    src/gui/ValueBlockModel.h \
    src/gui/ValueDock.h \
    src/gui/VectorBlockModel.h \
    src/gui/XteDock.h \
    src/kinematic/CgalWorker.h \
    src/kinematic/FixedKinematic.h \
    src/kinematic/GeographicConvertionWrapper.h \
    src/kinematic/PathPrimitive.h \
    src/kinematic/PoseOptions.h \
    src/kinematic/TrailerKinematic.h

#android {
    HEADERS += \
        src/cgal.h \
        src/cgalKernel.h
#}
#!android {
#    PRECOMPILED_HEADER += \
#        src/cgal.h \
#        src/cgalKernel.h
#}

FORMS += \
    src/gui/CameraToolbar.ui \
    src/gui/FieldsOptimitionToolbar.ui \
    src/gui/FieldsToolbar.ui \
    src/gui/GuidanceToolbar.ui \
    src/gui/GuidanceTurning.ui \
    src/gui/PassToolbar.ui \
    src/gui/SettingsDialog.ui \
    src/gui/SimulatorToolbar.ui \
    src/gui/ThreeValuesDock.ui \
    src/gui/ValueDock.ui \
    src/gui/XteDock.ui \
    src/gui/lengthwidget.ui \
    src/gui/vectorwidget.ui

RESOURCES += \
    res/ressources.qrc

!linux | android {
    RESOURCES += \
        lib/oxygen-icons-png/oxygen.qrc
}

include(build-paths.pri)

include($$PWD/src/qnodeseditor/qnodeeditor.pri)
include($$PWD/lib/geographiclib.pri)
include($$PWD/lib/cgal.pri)

# KDDockWidgets
INCLUDEPATH += $$KDDOCKWIDGET_PREFIX/include/
LIBS += -L$$KDDOCKWIDGET_PREFIX/lib -lkddockwidgets

unix {
target.path = /opt/QtOpenGuidance
configs.path = /opt/QtOpenGuidance/config
}

android {
QT += androidextras

configs.path = $$DATADIR/configs
}

INSTALLS += target

configs.files = config/*
INSTALLS += configs

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
