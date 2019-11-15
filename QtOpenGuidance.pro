QT += 3dcore 3drender 3dinput 3dextras
QT += widgets core

!android {
    QT += serialport
    DEFINES += SERIALPORT_ENABLED
    HEADERS += src/block/SerialPort.h
}

android {
    DEFINES += ANDROID_ENABLED
}

SOURCES += src/main.cpp \
    src/3d/linemesh.cpp \
    src/3d/linemeshgeometry.cpp \
    src/3d/pointmesh.cpp \
    src/3d/pointmeshgeometry.cpp \
    src/block/PoseSimulation.cpp \
    src/block/TractorModel.cpp \
    src/block/TrailerModel.cpp \
    src/gui/CameraToolbar.cpp \
    src/gui/GuidanceMeterBar.cpp \
    src/gui/GuidanceToolbar.cpp \
    src/gui/GuidanceTurning.cpp \
    src/gui/GuidanceXteBar.cpp \
    src/gui/ImplementBlockModel.cpp \
    src/gui/ImplementSectionModel.cpp \
    src/gui/NumberBlockModel.cpp \
    src/gui/PassToolbar.cpp \
    src/gui/SectionControlToolbar.cpp \
    src/gui/SettingsDialog.cpp \
    src/gui/SimulatorToolbar.cpp \
    src/gui/StringBlockModel.cpp \
    src/gui/VectorBlockModel.cpp
    
HEADERS += \
    src/3d/ArrowTexture.h \
    src/3d/linemeshgeometry.h \
    src/3d/linemesh.h \
    src/3d/pointmesh.h \
    src/3d/pointmeshgeometry.h \
    src/block/AckermannSteering.h \
    src/block/BlockBase.h \
    src/block/CameraController.h \
    src/block/CommunicationJrk.h \
    src/block/CommunicationPgn7FFE.h \
    src/block/DebugSink.h \
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
    src/block/MeterBarModel.h \
    src/block/NmeaParserGGA.h \
    src/block/NmeaParserHDT.h \
    src/block/NmeaParserRMC.h \
    src/block/NumberObject.h \
    src/block/PoseSimulation.h \
    src/block/PoseSynchroniser.h \
    src/block/PrintLatency.h \
    src/block/SectionControlModel.h \
    src/block/StringObject.h \
    src/block/TractorModel.h \
    src/block/TrailerModel.h \
    src/block/TransverseMercatorConverter.h \
    src/block/UdpSocket.h \
    src/block/VectorObject.h \
    src/block/XteBarModel.h \
    src/gui/CameraToolbar.h \
    src/gui/GuidanceMeterBar.h \
    src/gui/GuidanceToolbar.h \
    src/gui/GuidanceTurning.h \
    src/gui/GuidanceXteBar.h \
    src/gui/ImplementBlockModel.h \
    src/gui/ImplementSectionModel.h \
    src/gui/MainWindow.h \
    src/gui/NumberBlockModel.h \
    src/gui/PassToolbar.h \
    src/gui/SectionControlToolbar.h \
    src/gui/SettingsDialog.h \
    src/gui/SimulatorToolbar.h \
    src/gui/StringBlockModel.h \
    src/gui/VectorBlockModel.h \
    src/kinematic/FixedKinematic.h \
    src/kinematic/PathPrimitive.h \
    src/kinematic/PoseOptions.h \
    src/kinematic/Tile.h \
    src/kinematic/TrailerKinematic.h

FORMS += \
    src/gui/CameraToolbar.ui \
    src/gui/GuidanceMeterBar.ui \
    src/gui/GuidanceToolbar.ui \
    src/gui/GuidanceTurning.ui \
    src/gui/GuidanceXteBar.ui \
    src/gui/PassToolbar.ui \
    src/gui/SectionControlToolbar.ui \
    src/gui/SettingsDialog.ui \
    src/gui/SimulatorToolbar.ui \
    src/gui/lengthwidget.ui \
    src/gui/vectorwidget.ui

RESOURCES += \
    res/ressources.qrc

# aspect
SOURCES += \
    src/aspect/fpsaspect.cpp \
    src/aspect/fpsmonitor.cpp \
    src/aspect/fpsmonitorbackend.cpp \
    src/aspect/movingaverage.cpp \
    src/aspect/updatefpsmonitorsjob.cpp

HEADERS += \
    src/aspect/fpsaspect.h \
    src/aspect/fpsmonitor.h \
    src/aspect/fpsmonitorbackend.h \
    src/aspect/movingaverage.h \
    src/aspect/updatefpsmonitorsjob.h


include($$PWD/src/qnodeseditor/qnodeeditor.pri)
include($$PWD/lib/geographiclib.pri)

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

