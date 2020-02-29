DEPENDPATH      += $$PWD

INCLUDEPATH     += $$PWD/CGAL-5.0/include

#DEFINES += "CGAL_DIR=$$PWD/cgal"

SOURCES +=

HEADERS  += $$PWD/CGAL-5.0/include/CGAL/*

!android {
LIBS += -lgmp -lmpfr
QMAKE_CXXFLAGS += -frounding-math
}

android {
LIBS += -L$$GMPMPFR_LIBPATH -lgmp -lmpfr
INCLUDEPATH += $$GMPMPFR_INLCUDE
ANDROID_EXTRA_LIBS += $${GMPMPFR_LIBPATH}/libgmp.so $${GMPMPFR_LIBPATH}/libmpfr.so
}

DEFINES+=CGAL_DISABLE_ROUNDING_MATH_CHECK=ON
