DEPENDPATH      += $$PWD

INCLUDEPATH     += $$PWD/CGAL-5.0/include

#DEFINES += "CGAL_DIR=$$PWD/cgal"

SOURCES +=

HEADERS  += $$PWD/CGAL-5.0/include/CGAL/*

# GPM/MPFR
!android {
LIBS += -lgmp -lmpfr
QMAKE_CXXFLAGS += -frounding-math
}

# GPM/MPFR
android {
LIBS += -L$$GMPMPFR_LIBPATH -lgmp -lmpfr
INCLUDEPATH += $$GMPMPFR_INLCUDE
ANDROID_EXTRA_LIBS += $${GMPMPFR_LIBPATH}/libgmp.so $${GMPMPFR_LIBPATH}/libmpfr.so
}

# boost
android {
    INCLUDEPATH += android/arm64-v8a//include/boost-1_70/
    LIBS += -Landroid/arm64-v8a/lib/
    QMAKE_CXXFLAGS += -fexceptions
    QMAKE_CXXFLAGS += -frtti
}

DEFINES+=CGAL_DISABLE_ROUNDING_MATH_CHECK=ON
