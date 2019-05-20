android|ios|tvos|winrt {
    warning( "This example is not supported for android, ios, tvos, or winrt." )
}

QT += 3dcore 3drender 3dinput 3dextras
QT += widgets core

SOURCES += src/main.cpp \
    src/AckermannKinematic.cpp \
    src/TractorModel.cpp \
    src/PoseSimulation.cpp \
    src/gui/guidancetoolbar.cpp \
    src/gui/settingsdialog.cpp \
    src/gui/simulatortoolbar.cpp
    
HEADERS += \
    src/AckermannKinematic.h \
    src/TractorModel.h \
    src/PoseCache.h \
    src/PoseSimulation.h \
    src/cameracontroller.h \
    src/gui/guidancetoolbar.h \
    src/gui/settingsdialog.h \
    src/gui/simulatortoolbar.h

QMAKE_CXXFLAGS += -pg -g

FORMS += \
    src/gui/guidancetoolbar.ui \
    src/gui/settingsdialog.ui \
    src/gui/simulatortoolbar.ui

RESOURCES += \
    res/ressources.qrc
