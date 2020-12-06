DEPENDPATH += $$PWD

INCLUDEPATH += $$PWD/CGAL-5.1.1/include

CONFIG(release) {
  DEFINES += CGAL_KERNEL_NO_PRECONDITIONS
}

# GPM/MPFR
!android {
    LIBS += -lgmp -lmpfr
    QMAKE_CXXFLAGS += -frounding-math
}

win32 {
    LIBS += -lgmp
}

# GPM/MPFR
android {
    LIBS += -L$$GMPMPFR_LIBPATH -lgmp -lmpfr
    INCLUDEPATH += $$GMPMPFR_INLCUDE
    ANDROID_EXTRA_LIBS += $${GMPMPFR_LIBPATH}/libgmp.so $${GMPMPFR_LIBPATH}/libmpfr.so
}

# boost
android {
    INCLUDEPATH += android/armeabi-v7a/include/boost-1_70/
    LIBS += -Landroid/armeabi-v7aa/lib/
    QMAKE_CXXFLAGS += -fexceptions
    QMAKE_CXXFLAGS += -frtti
}

DEFINES+=CGAL_DISABLE_ROUNDING_MATH_CHECK=ON
