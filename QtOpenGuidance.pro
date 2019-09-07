QT += 3dcore 3drender 3dinput 3dextras serialport
QT += widgets core

SOURCES += src/main.cpp \
    src/3d/linemesh.cpp \
    src/3d/linemeshgeometry.cpp \
    src/3d/pointmesh.cpp \
    src/3d/pointmeshgeometry.cpp \
    src/block/PoseSimulation.cpp \
    src/block/TractorModel.cpp \
    src/block/TrailerModel.cpp \
    src/gui/CameraToolbar.cpp \
    src/gui/GuidanceToolbar.cpp \
    src/gui/GuidanceToolbarTop.cpp \
    src/gui/GuidanceXteBar.cpp \
    src/gui/NumberBlockModel.cpp \
    src/gui/SectionControlToolbar.cpp \
    src/gui/SettingsDialog.cpp \
    src/gui/SimulatorToolbar.cpp \
    src/gui/StringBlockModel.cpp \
    src/gui/VectorBlockModel.cpp
    
HEADERS += \
    src/3d/linemeshgeometry.h \
    src/3d/linemesh.h \
    src/3d/pointmesh.h \
    src/3d/pointmeshgeometry.h \
    src/block/AckermannSteering.h \
    src/block/BlockBase.h \
    src/block/CameraController.h \
    src/block/DebugSink.h \
    src/block/FileStream.h \
    src/block/GridModel.h \
    src/block/GuidanceGlobalPlanner.h \
    src/block/GuidanceLocalPlanner.h \
    src/block/GuidancePlannerGui.h \
    src/block/GuidanceStanley.h \
    src/block/GuidanceXte.h \
    src/block/NmeaParser.h \
    src/block/NumberObject.h \
    src/block/PathPrimitive.h \
    src/block/PoseSimulation.h \
    src/block/PoseSynchroniser.h \
    src/block/PrintLatency.h \
    src/block/SectionControlModel.h \
    src/block/SerialPort.h \
    src/block/StringObject.h \
    src/block/TractorModel.h \
    src/block/TrailerModel.h \
    src/block/TransverseMercatorConverter.h \
    src/block/UdpSocket.h \
    src/block/VectorObject.h \
    src/block/XteBarModel.h \
    src/gui/CameraToolbar.h \
    src/gui/GuidanceToolbar.h \
    src/gui/GuidanceToolbarTop.h \
    src/gui/GuidanceXteBar.h \
    src/gui/MainWindow.h \
    src/gui/NumberBlockModel.h \
    src/gui/SectionControlToolbar.h \
    src/gui/SettingsDialog.h \
    src/gui/SimulatorToolbar.h \
    src/gui/StringBlockModel.h \
    src/gui/VectorBlockModel.h \
    src/kinematic/Field.h \
    src/kinematic/FixedKinematic.h \
    src/kinematic/PoseOptions.h \
    src/kinematic/Tile.h \
    src/kinematic/TrailerKinematic.h

FORMS += \
    src/gui/CameraToolbar.ui \
    src/gui/GuidanceToolbar.ui \
    src/gui/GuidanceToolbarTop.ui \
    src/gui/GuidanceXteBar.ui \
    src/gui/SectionControlToolbar.ui \
    src/gui/SettingsDialog.ui \
    src/gui/SimulatorToolbar.ui \
    src/gui/lengthwidget.ui \
    src/gui/vectorwidget.ui

RESOURCES += \
    res/ressources.qrc

#in your project main .pro qmake configuration file
#include($$PWD/lib/QuickQanava/src/quickqanava.pri)

include($$PWD/src/qnodeseditor/qnodeeditor.pri)
include($$PWD/lib/geographiclib.pri)

