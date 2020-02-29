QT          += core widgets gui

DEPENDPATH      += $$PWD
INCLUDEPATH     += $$PWD

SOURCES += \
    $$PWD/qneblock.cpp \
    $$PWD/qneport.cpp \
    $$PWD/qneconnection.cpp \
    $$PWD/qnodeseditor.cpp

HEADERS += \
    $$PWD/qneblock.h \
    $$PWD/qnegestures.h \
    $$PWD/qnegraphicsview.h \
    $$PWD/qneport.h \
    $$PWD/qneconnection.h \
    $$PWD/qnodeseditor.h

SOURCES += \
    $$PWD/qnodeseditor-moc.cpp
