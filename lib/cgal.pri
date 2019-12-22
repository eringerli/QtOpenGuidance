DEPENDPATH      += $$PWD

INCLUDEPATH     += $$PWD/CGAL-5.0/include

#DEFINES += "CGAL_DIR=$$PWD/cgal"

SOURCES +=

HEADERS  += $$PWD/CGAL-5.0/include/CGAL/*

!android {
LIBS += -lgmp -lmpfr
QMAKE_CXXFLAGS += -frounding-math
}
