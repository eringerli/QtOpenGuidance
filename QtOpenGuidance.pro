QT += 3dcore 3drender 3dinput 3dextras
QT += widgets core

SOURCES += src/main.cpp \
    src/3d/TractorModel.cpp \
    src/3d/TrailerModel.cpp \
    src/PoseSimulation.cpp \
    src/gui/guidancetoolbar.cpp \
    src/gui/settingsdialog.cpp \
    src/gui/simulatortoolbar.cpp
    
HEADERS += \
    src/3d/TractorModel.h \
    src/3d/TrailerModel.h \
    src/3d/cameracontroller.h \
    src/GuidanceBase.h \
    src/PoseCache.h \
    src/PoseSimulation.h \
    src/gui/guidancetoolbar.h \
    src/gui/settingsdialog.h \
    src/gui/simulatortoolbar.h \
    src/kinematic/FixedKinematic.h \
    src/kinematic/TrailerKinematic.h

#QMAKE_CXXFLAGS += -pg -g

FORMS += \
    src/gui/guidancetoolbar.ui \
    src/gui/settingsdialog.ui \
    src/gui/simulatortoolbar.ui

RESOURCES += \
    res/ressources.qrc

#in your project main .pro qmake configuration file
#include($$PWD/lib/QuickQanava/src/quickqanava.pri)

include($$PWD/src/qnodeseditor/qnodeeditor.pri)
