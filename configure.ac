AC_PREREQ(2.59)
AC_INIT(musrfit, 0.6.0, andreas.suter@psi.ch)
AC_CONFIG_AUX_DIR(admin)
AC_CANONICAL_HOST
#AC_MSG_RESULT([${host} ${host_cpu} ${host_vendor} ${host_os}])

dnl -----------------------------------------------
dnl Check if pkg-config is installed
dnl -----------------------------------------------

m4_ifdef([PKG_CHECK_MODULES],[],AC_MSG_ERROR([Please install pkg-config before configuring musrfit!]))

dnl -----------------------------------------------
dnl Package names and version numbers
dnl -----------------------------------------------

MUSR_PROGRAM_NAME=musrfit
MUSR_LIBRARY_NAME=PMusr
LEM_LIBRARY_NAME=TLemRunHeader
PSIBIN_LIBRARY_NAME=Class_MuSR_PSI
MUD_LIBRARY_NAME=mud
CUBA_LIBRARY_NAME=cuba

dnl -----------------------------------------------
dnl Optional plug-ins
dnl -----------------------------------------------


#release versioning
MUSR_MAJOR_VERSION=0
MUSR_MINOR_VERSION=6
MUSR_MICRO_VERSION=0

#release versioning
LEM_MAJOR_VERSION=1
LEM_MINOR_VERSION=5
LEM_MICRO_VERSION=0

#release versioning
PSIBIN_MAJOR_VERSION=0
PSIBIN_MINOR_VERSION=1
PSIBIN_MICRO_VERSION=0

#release versioning
MUD_MAJOR_VERSION=0
MUD_MINOR_VERSION=0
MUD_MICRO_VERSION=0

#release versioning
PLUGIN_MAJOR_VERSION=1
PLUGIN_MINOR_VERSION=0
PLUGIN_MICRO_VERSION=0

#release versioning
CUBA_MAJOR_VERSION=1
CUBA_MINOR_VERSION=6
CUBA_MICRO_VERSION=0

#API version
MUSR_API_VERSION=MUSR_MAJOR_VERSION.MUSR_MINOR_VERSION
AC_SUBST(MUSR_API_VERSION)

LEM_API_VERSION=LEM_MAJOR_VERSION.LEM_MINOR_VERSION
AC_SUBST(LEM_API_VERSION)

PSIBIN_API_VERSION=PSIBIN_MAJOR_VERSION.PSIBIN_MINOR_VERSION
AC_SUBST(PSIBIN_API_VERSION)

MUD_API_VERSION=MUD_MAJOR_VERSION.MUD_MINOR_VERSION
AC_SUBST(MUD_API_VERSION)

PLUGIN_API_VERSION=PLUGIN_MAJOR_VERSION.PLUGIN_MINOR_VERSION
AC_SUBST(PLUGIN_API_VERSION)

CUBA_API_VERSION=CUBA_MAJOR_VERSION.CUBA_MINOR_VERSION
AC_SUBST(CUBA_API_VERSION)

#shared library versioning
CUBA_LIBRARY_VERSION=1:6:0
PLUGIN_LIBRARY_VERSION=1:0:0
LEM_LIBRARY_VERSION=1:5:0
PSIBIN_LIBRARY_VERSION=0:1:0
MUD_LIBRARY_VERSION=0:0:0
MUSR_LIBRARY_VERSION=0:6:0
#                    | | |
#             +------+ | +---+
#             |        |     |
#          current:revision:age
#             |        |     |
#             |        |     +- increment if interfaces have been added
#             |        |        set to zero if interfaces have been removed
#                               or changed
#             |        +- increment if source code has changed
#             |           set to zero if current is incremented
#             +- increment if interfaces have been added, removed or changed

AC_SUBST(MUSR_LIBRARY_VERSION)
AC_SUBST(LEM_LIBRARY_VERSION)
AC_SUBST(PSIBIN_LIBRARY_VERSION)
AC_SUBST(MUD_LIBRARY_VERSION)
AC_SUBST(PLUGIN_LIBRARY_VERSION)
AC_SUBST(CUBA_LIBRARY_VERSION)

PACKAGE=$MUSR_PROGRAM_NAME
AC_SUBST(MUSR_LIBRARY_NAME)
AC_SUBST(LEM_LIBRARY_NAME)
AC_SUBST(PSIBIN_LIBRARY_NAME)
AC_SUBST(MUD_LIBRARY_NAME)
AC_SUBST(CUBA_LIBRARY_NAME)

PSIBIN_VERSION=$PSIBIN_MAJOR_VERSION.$PSIBIN_MINOR_VERSION.$PSIBIN_MICRO_VERSION
PSIBIN_RELEASE=$PSIBIN_MAJOR_VERSION.$PSIBIN_MINOR_VERSION
AC_SUBST(PSIBIN_RELEASE)
AC_SUBST(PSIBIN_VERSION)

MUD_VERSION=$MUD_MAJOR_VERSION.$MUD_MINOR_VERSION.$MUD_MICRO_VERSION
MUD_RELEASE=$MUD_MAJOR_VERSION.$MUD_MINOR_VERSION
AC_SUBST(MUD_RELEASE)
AC_SUBST(MUD_VERSION)

LEM_VERSION=$LEM_MAJOR_VERSION.$LEM_MINOR_VERSION.$LEM_MICRO_VERSION
LEM_RELEASE=$LEM_MAJOR_VERSION.$LEM_MINOR_VERSION
AC_SUBST(LEM_RELEASE)
AC_SUBST(LEM_VERSION)

MUSR_VERSION=$MUSR_MAJOR_VERSION.$MUSR_MINOR_VERSION.$MUSR_MICRO_VERSION
MUSR_RELEASE=$MUSR_MAJOR_VERSION.$MUSR_MINOR_VERSION
AC_SUBST(MUSR_RELEASE)
AC_SUBST(MUSR_VERSION)

PLUGIN_VERSION=$PLUGIN_MAJOR_VERSION.$PLUGIN_MINOR_VERSION.$PLUGIN_MICRO_VERSION
PLUGIN_RELEASE=$PLUGIN_MAJOR_VERSION.$PLUGIN_MINOR_VERSION
AC_SUBST(PLUGIN_RELEASE)
AC_SUBST(PLUGIN_VERSION)
AC_SUBST(PLUGIN_MAJOR_VERSION)
AC_SUBST(PLUGIN_MINOR_VERSION)

CUBA_VERSION=$CUBA_MAJOR_VERSION.$CUBA_MINOR_VERSION.$CUBA_MICRO_VERSION
CUBA_RELEASE=$CUBA_MAJOR_VERSION.$CUBA_MINOR_VERSION
AC_SUBST(CUBA_RELEASE)
AC_SUBST(CUBA_VERSION)

VERSION=$MUSR_VERSION

USER_CFLAGS="$CFLAGS"

dnl -----------------------------------------------
dnl Automake initialization and program checks
dnl -----------------------------------------------

AM_INIT_AUTOMAKE(@PACKAGE_NAME@, @PACKAGE_VERSION@)
AC_CONFIG_HEADER([config.h])
AC_LANG([C++])
AC_PROG_LN_S
AC_PROG_INSTALL

m4_ifdef([LT_INIT],
         [LT_INIT([disable-static])
          LT_LANG([C++])
          LT_LANG([C])],
         [m4_defun([_LT_AC_LANG_F77_CONFIG], [:])
          AM_DISABLE_STATIC
          AM_PROG_LIBTOOL
          AC_PROG_CXX
          AC_PROG_CC])

dnl -----------------------------------------------
dnl Ask user for path to fftw3
dnl -----------------------------------------------

FFTW3_FOUND=0
AC_ARG_WITH([fftw3],
    [AC_HELP_STRING([--with-fftw3],[prefix of FFTW3 installation, e.g. /usr/local or /usr])],
    [FFTW3_PREFIX=$with_fftw3
     AC_MSG_CHECKING([whether FFTW3 can be found at the specified location])
     if !(test -r ${FFTW3_PREFIX}/include/fftw3.h); then
       AC_MSG_RESULT([no])
       AC_MSG_ERROR([FFTW3 cannot be found at the specified path!])
     fi
     AC_MSG_RESULT([${FFTW3_PREFIX}])],
    [PKG_CHECK_MODULES(FFTW3, fftw3 >= 3.1.0, [FFTW3_FOUND=1],
     [AC_MSG_CHECKING([whether FFTW3 is installed in a standard location])
      if test -r /usr/local/include/fftw3.h; then
        FFTW3_PREFIX="/usr/local"
        AC_MSG_RESULT([${FFTW3_PREFIX}])
      elif test -r /usr/include/fftw3.h; then
        FFTW3_PREFIX="/usr"
        AC_MSG_RESULT([${FFTW3_PREFIX}])
      elif test -r /sw/include/fftw3.h; then
        FFTW3_PREFIX="/sw"
        AC_MSG_RESULT([${FFTW3_PREFIX}])
      elif test -r /opt/local/include/fftw3.h; then
        FFTW3_PREFIX="/opt/local"
        AC_MSG_RESULT([${FFTW3_PREFIX}])
      else
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(
        [FFTW3 not found. Please call configure with the --with-fftw3 option.
        This tells configure where to find the FFTW3 C libraries and headers,
        e.g. --with-fftw3=/usr/local or --with-fftw3=/usr]
        )
      fi
     ]
    )
    ]
)
AC_SUBST(FFTW3_PREFIX)
if test "${FFTW3_FOUND}" != "1"; then
  FFTW3_LIBS="-L${FFTW3_PREFIX}/lib -lfftw3 -lm"
  FFTW3_CFLAGS="-I${FFTW3_PREFIX}/include"
fi

dnl -----------------------------------------------
dnl Ask user for path to gsl
dnl -----------------------------------------------

GSL_FOUND=0
AC_ARG_WITH([gsl],
    [AC_HELP_STRING([--with-gsl],[prefix of GSL installation. e.g. /usr/local or /usr])],
    [GSL_PREFIX=$with_gsl
     AC_MSG_CHECKING([whether GSL can be found at the specified location])
     if !(test -r ${GSL_PREFIX}/include/gsl/gsl_sf_hyperg.h); then
       AC_MSG_RESULT([no])
       AC_MSG_ERROR([GSL cannot be found at the specified path!])
     fi
     AC_MSG_RESULT([${GSL_PREFIX}])],
    [PKG_CHECK_MODULES(GSL, gsl >= 1.9, [GSL_FOUND=1],
     [AC_MSG_CHECKING([whether GSL is installed in a standard location])
      if test -r /usr/local/include/gsl/gsl_sf_hyperg.h; then
        GSL_PREFIX="/usr/local"
        AC_MSG_RESULT([${GSL_PREFIX}])
      elif test -r /usr/include/gsl/gsl_sf_hyperg.h; then
        GSL_PREFIX="/usr"
        AC_MSG_RESULT([${GSL_PREFIX}])
      elif test -r /sw/include/gsl/gsl_sf_hyperg.h; then
        GSL_PREFIX="/sw"
        AC_MSG_RESULT([${GSL_PREFIX}])
      elif test -r /opt/local/include/gsl/gsl_sf_hyperg.h; then
        GSL_PREFIX="/opt/local"
        AC_MSG_RESULT([${GSL_PREFIX}])
      else
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(
        [GSL not found. Please call configure with the --with-gsl option.
        This tells configure where to find the GSL C libraries and headers,
        e.g. --with-gsl=/usr/local or --with-gsl=/usr]
        )
      fi
     ]
    )
    ]
)
AC_SUBST(GSL_PREFIX)
if test "${GSL_FOUND}" != "1"; then
  GSL_LIBS="-L${GSL_PREFIX}/lib -lgsl -lgslcblas -lm"
  GSL_CFLAGS="-I${GSL_PREFIX}/include/gsl"
fi
AC_SUBST(GSL_LIBS)
AC_SUBST(GSL_CFLAGS)


dnl -----------------------------------------------
dnl Ask user for path to boost
dnl -----------------------------------------------

BOOST_FOUND=0
AC_ARG_WITH([boostinc],
    [AC_HELP_STRING([--with-boostinc],[path to the header files of the BOOST installation, e.g. /usr/local/include or /usr/include])],
    [BOOST_INCLUDE=$with_boostinc
     AC_MSG_CHECKING([whether BOOST can be found at the specified location])
     if !(test -r ${BOOST_INCLUDE}/boost/spirit.hpp) && !(test -r ${BOOST_INCLUDE}/boost/spirit/include/classic_spirit.hpp); then
       AC_MSG_RESULT([no])
       AC_MSG_ERROR([BOOST cannot be found at the specified path!])
     fi
     AC_MSG_RESULT([${BOOST_INCLUDE}])],
    [PKG_CHECK_MODULES(BOOST, boost >= 1.30, [BOOST_FOUND=1],
     [AC_MSG_CHECKING([whether BOOST is installed in a standard location])
      if test -r /usr/local/include/boost/spirit.hpp || test -r /usr/local/include/boost/spirit/include/classic_spirit.hpp; then
        BOOST_INCLUDE="/usr/local/include"
        AC_MSG_RESULT([${BOOST_INCLUDE}])
      elif test -r /usr/include/boost/spirit.hpp || test -r /usr/include/boost/spirit/include/classic_spirit.hpp; then
        BOOST_INCLUDE="/usr/include"
        AC_MSG_RESULT([${BOOST_INCLUDE}])
      elif test -r /sw/include/boost/spirit.hpp || test -r /sw/include/boost/spirit/include/classic_spirit.hpp; then
        BOOST_INCLUDE="/sw/include"
        AC_MSG_RESULT([${BOOST_INCLUDE}])
      elif test -r /opt/local/include/boost/spirit.hpp || test -r /opt/local/include/boost/spirit/include/classic_spirit.hpp; then
        BOOST_INCLUDE="/opt/local/include"
        AC_MSG_RESULT([${BOOST_INCLUDE}])
      else
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(
        [BOOST not found. Please call configure with the --with-boostinc option.
        This tells configure where to find the BOOST C++ headers,
        e.g. --with-boostinc=/usr/local/include or --with-boostinc=/usr/include or --with-boostinc=/usr/include/boost-1_33_0]
        )
      fi
     ]
    )
    ]
)
AC_SUBST(BOOST_INCLUDE)
if test "${BOOST_FOUND}" != "1"; then
  BOOST_LIBS=""
  BOOST_CFLAGS="-I${BOOST_INCLUDE}"
fi
AC_SUBST(BOOST_LIBS)
AC_SUBST(BOOST_CFLAGS)


dnl -----------------------------------------------
dnl Check for ROOT
dnl -----------------------------------------------

ROOT_PATH([5.22/00], [], AC_MSG_ERROR([Either ROOT is not installed correctly or the version is too old... please check!]))

ROOT_LIBS="-L${ROOTLIBDIR} ${ROOTGLIBS} ${ROOTAUXLIBS} -lMinuit2 -lMathMore -lXMLParser"
ROOT_CFLAGS="-I${ROOTINCDIR} ${ROOTCFLAGS} ${ROOTAUXCFLAGS}"

# Check if Minuit2 has been enabled during the ROOT configuration

AC_MSG_CHECKING([for Minuit2 in ROOT])
if [[[ $(${ROOTCONF} --config) == *--enable-minuit2* ]]]; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([Minuit2 not included in the ROOT installation... Please configure ROOT with the --enable-minuit2 option!])
fi

AC_SUBST(ROOT_LIBS)
AC_SUBST(ROOT_CFLAGS)


dnl -----------------------------------------------
dnl Ask user if BMWlibs should be built and where to find the CUBA installation
dnl -----------------------------------------------

AC_ARG_ENABLE([BMWlibs], [AC_HELP_STRING([--enable-BMWlibs],[build optional BMW plug-ins [default=no]])],
    [CUBA_FOUND=0
    BUILD_CUBA=0
    AC_ARG_WITH([cuba],
        [AC_HELP_STRING([--with-cuba],[prefix of CUBA installation, e.g. /usr/local or /usr])],
        [CUBA_PREFIX=$with_cuba
        AC_MSG_CHECKING([whether CUBA can be found at the specified location])
        if !(test -r ${CUBA_PREFIX}/include/cuba.h); then
          AC_MSG_RESULT([no])
          AC_MSG_ERROR([CUBA cannot be found at the specified path!])
        fi
        AC_MSG_RESULT([${CUBA_PREFIX}])],
        [PKG_CHECK_MODULES(CUBA, cuba >= 1.6, [CUBA_FOUND=1],
          [AC_MSG_CHECKING([whether CUBA is installed in a standard location])
          if test -r /usr/local/include/cuba.h; then
            CUBA_PREFIX="/usr/local"
            AC_MSG_RESULT([${CUBA_PREFIX}])
          elif test -r /usr/include/cuba.h; then
            CUBA_PREFIX="/usr"
            AC_MSG_RESULT([${CUBA_PREFIX}])
          elif test -r /sw/include/cuba.h; then
            CUBA_PREFIX="/sw"
            AC_MSG_RESULT([${CUBA_PREFIX}])
          elif test -r /opt/local/include/cuba.h; then
            CUBA_PREFIX="/opt/local"
            AC_MSG_RESULT([${CUBA_PREFIX}])
          else
            BUILD_CUBA=1
            AC_MSG_RESULT([builtin-cuba])
          fi
        ]
      )
      ]
    )

    if test "${BUILD_CUBA}" = "1"; then
      if test "x$GCC" = "xyes" ; then
        CUBA_BUILD_CFLAGS=${USER_CFLAGS:--O3 -fomit-frame-pointer -ffast-math}
      else
        CUBA_BUILD_CFLAGS=${USER_CFLAGS:--O}
      fi

      AC_LANG_PUSH([C])

      AC_C_CONST
      AC_C_INLINE
      AC_C_LONG_DOUBLE

      AC_CHECK_FUNCS([powl])
      AC_CHECK_FUNCS([erf])

      MAXDIM=${MAXDIM:-16}
      AC_ARG_WITH(maxdim,
        [AS_HELP_STRING([--with-maxdim=N],
        [[Cuba option] the maximum dimension for integration,
          if variable-size array are not supported])],
        [MAXDIM=$withval])

      MAXCOMP=${MAXCOMP:-4}
      AC_ARG_WITH(maxcomp,
        [AS_HELP_STRING([--with-maxcomp=N],
        [[Cuba option] the maximum number of components of the integrand,
          if variable-size array are not supported])],
        [MAXCOMP=$withval])

      AC_MSG_CHECKING([for variable-size arrays])
      AC_COMPILE_IFELSE([AC_LANG_SOURCE(,[[
        void test(int n)
        {
          char s[n];
        }
      ]])],
      [AC_MSG_RESULT([yes])],
      [AC_MSG_RESULT([no, using MAXDIM=$MAXDIM and MAXCOMP=$MAXCOMP])
       AC_DEFINE_UNQUOTED([NDIM], [$MAXDIM], [Maximum number of components])
       AC_DEFINE_UNQUOTED([NCOMP], [$MAXCOMP], [Maximum number of dimensions])]
      )

      AC_LANG_POP([C])

      CUBA_SRCDIR="$(pwd)/src/external/libCuba/src"
      CUBA_LIBS="${CUBA_SRCDIR}/libcuba.la"
      CUBA_CFLAGS="-I${CUBA_SRCDIR}"
    else
      if test "${CUBA_FOUND}" != "1"; then
        CUBA_LIBS="-L${CUBA_PREFIX}/lib -lcuba -lm"
        CUBA_CFLAGS="-I${CUBA_PREFIX}/include"
      fi
    fi

    AC_SUBST(CUBA_PREFIX)
    AC_SUBST(CUBA_LIBS)
    AC_SUBST(CUBA_CFLAGS)

    BUILD_BMW_LIBS=1
    ],
    [BUILD_BMW_LIBS=0]
)

dnl -----------------------------------------------
dnl Ask user if ASlibs should be built
dnl -----------------------------------------------

AC_ARG_ENABLE([ASlibs], [AC_HELP_STRING([--enable-ASlibs],[build optional AS plug-ins [default=no]])],[BUILD_AS_LIBS=1], [BUILD_AS_LIBS=0])

dnl -----------------------------------------------
dnl Set some paths and flags for PMusr, TLemRunHeader, Class_MuSR_PSI and mud
dnl -----------------------------------------------

SRCDIR="$(pwd)/src"

PSIBIN_SRCDIR="${SRCDIR}/external/MuSR_software/Class_MuSR_PSI"
PSIBIN_LIBS="${PSIBIN_SRCDIR}/lib${PSIBIN_LIBRARY_NAME}.la"
PSIBIN_CFLAGS="-I${PSIBIN_SRCDIR}"
AC_SUBST(PSIBIN_LIBS)
AC_SUBST(PSIBIN_CFLAGS)

MUD_SRCDIR="${SRCDIR}/external/mud/src"
MUD_LIBS="${MUD_SRCDIR}/lib${MUD_LIBRARY_NAME}.la"
MUD_CFLAGS="-I${MUD_SRCDIR}"
AC_SUBST(MUD_LIBS)
AC_SUBST(MUD_CFLAGS)

LEM_SRCDIR="${SRCDIR}/external/TLemRunHeader"
LEM_LIBS="${LEM_SRCDIR}/lib${LEM_LIBRARY_NAME}.la"
LEM_CFLAGS="-I${LEM_SRCDIR}"
AC_SUBST(LEM_LIBS)
AC_SUBST(LEM_CFLAGS)

PMUSR_LIBS="${SRCDIR}/classes/lib${MUSR_LIBRARY_NAME}.la"
PMUSR_CFLAGS="-I${SRCDIR}/include"
AC_SUBST(PMUSR_LIBS)
AC_SUBST(PMUSR_CFLAGS)

if test "${BUILD_BMW_LIBS}" = "1"; then
  TFITPOFB_SRCDIR="${SRCDIR}/external/TFitPofB-lib"
  TFITPOFB_LIBS="${TFITPOFB_SRCDIR}/classes/libTFitPofB.la"
  TFITPOFB_CFLAGS="-I${TFITPOFB_SRCDIR}/include"
  AC_SUBST(TFITPOFB_LIBS)
  AC_SUBST(TFITPOFB_CFLAGS)

  # Check for fftw3_threads-library. If available musrfit is also linked against it (used in libTFitPofB).
  SAVED_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $FFTW3_CFLAGS"
  SAVED_LIBSS="$LIBS"
  LIBS="$LIBS $FFTW3_LIBS"
  AC_SEARCH_LIBS([fftw_init_threads], [fftw3_threads], [FFTW3_LIBS="$FFTW3_LIBS -lfftw3_threads -lpthread"
                                      AC_DEFINE([HAVE_LIBFFTW3_THREADS], [1], [Define to 1 if fftw3_threads are available])], [], [-lpthread])
  CFLAGS="$SAVED_CFLAGS"
  LIBS="$SAVED_LIBS"

  # Check for gomp-library. If available musrfit is also linked against it (used in libTFitPofB).
  SAVED_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -fopenmp"
  SAVED_LIBSS="$LIBS"
  LIBS="$LIBS -fopenmp"
  AC_SEARCH_LIBS([omp_get_num_procs], [gomp], [AC_DEFINE([HAVE_GOMP], [1], [Define to 1 if gomp is available])],
                                      [CXXFLAGS="$SAVED_CXXFLAGS" LIBS="$SAVED_LIBS"], [])
fi
AC_SUBST(FFTW3_LIBS)
AC_SUBST(FFTW3_CFLAGS)


dnl -----------------------------------------------
dnl Set host specific compiler and linker flags
dnl -----------------------------------------------

case "${host_cpu}" in
  x86_64)
    CPUFLAGS="-m64 -fPIC -DPIC"
    ;;
  *)
    CPUFLAGS=
    ;;
esac

LOCAL_BIN_CXXFLAGS="${CPUFLAGS} -Wall -Wno-trigraphs"
LOCAL_LIB_CXXFLAGS="${LOCAL_BIN_CXXFLAGS}"
LOCAL_PSIBIN_LIB_CXXFLAGS="${LOCAL_LIB_CXXFLAGS}"
LOCAL_MUD_LIB_CXXFLAGS="${LOCAL_LIB_CXXFLAGS}"
LOCAL_CUBA_LIB_CFLAGS="${LOCAL_LIB_CXXFLAGS} ${CUBA_BUILD_CFLAGS}"
LOCAL_BIN_LDFLAGS=
LOCAL_LIB_LDFLAGS=

case "$host" in
  *-*-cygwin)
    ARCH=CYGWIN
    LOCAL_BIN_CXXFLAGS="${LOCAL_BIN_CXXFLAGS}"
    LOCAL_LIB_CXXFLAGS="${LOCAL_BIN_CXXFLAGS} -D_DLL"
    LOCAL_PSIBIN_LIB_CXXFLAGS="${LOCAL_LIB_CXXFLAGS} -D_WIN32GCC"
    LOCAL_MUD_LIB_CXXFLAGS="${LOCAL_LIB_CXXFLAGS}"
    LOCAL_CUBA_LIB_CFLAGS="${LOCAL_LIB_CXXFLAGS} ${CUBA_BUILD_CFLAGS}"
    LOCAL_BIN_LDFLAGS="${LOCAL_BIN_LDFLAGS} -Wl,--enable-auto-import -Wl,--enable-runtime-pseudo-reloc"
    LOCAL_LIB_LDFLAGS="-no-undefined ${LOCAL_BIN_LDFLAGS} -Wl,--export-all-symbols"
    ;;
  *-*-darwin*)
    ARCH=DARWIN
    LOCAL_PSIBIN_LIB_CXXFLAGS="${LOCAL_LIB_CXXFLAGS} -D__linux__"
    ;;
  *)
    ARCH=OTHERUNIX
    LOCAL_PSIBIN_LIB_CXXFLAGS="${LOCAL_LIB_CXXFLAGS} -D__linux__"
    ;;
esac


AC_SUBST(LOCAL_BIN_CXXFLAGS)
AC_SUBST(LOCAL_LIB_CXXFLAGS)
AC_SUBST(LOCAL_PSIBIN_LIB_CXXFLAGS)
AC_SUBST(LOCAL_MUD_LIB_CXXFLAGS)
AC_SUBST(LOCAL_CUBA_LIB_CFLAGS)
AC_SUBST(LOCAL_BIN_LDFLAGS)
AC_SUBST(LOCAL_LIB_LDFLAGS)


dnl -----------------------------------------------
dnl Some standard checks for header files and libraries
dnl Since these checks are not further used at the moment, they are commented
dnl -----------------------------------------------

# Checks for header files.
#AC_HEADER_STDC
#AC_CHECK_HEADERS([stddef.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_HEADER_STDBOOL
#AC_C_CONST
#AC_C_INLINE
#AC_TYPE_SIZE_T
#AC_HEADER_TIME

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_STAT
#AC_CHECK_FUNCS([floor gettimeofday memset pow sqrt strstr])


dnl -----------------------------------------------
dnl Specify the files that are going to be created by configure
dnl -----------------------------------------------

AM_CONDITIONAL([IS_DARWIN], [test "${ARCH}" = "DARWIN"])
AM_CONDITIONAL([IS_CYGWIN], [test "${ARCH}" = "CYGWIN"])

AM_CONDITIONAL([BUILD_CUBALIB], [test "${BUILD_CUBA}" = "1"])
AM_CONDITIONAL([BUILD_BMWLIBS], [test "${BUILD_BMW_LIBS}" = "1"])
AM_CONDITIONAL([BUILD_ASLIBS], [test "${BUILD_AS_LIBS}" = "1"])

AC_CONFIG_FILES([Makefile \
                 src/Makefile \
                 src/classes/Makefile \
                 src/classes/PMusr.pc \
                 src/external/Makefile \
                 src/external/TLemRunHeader/Makefile \
                 src/external/TLemRunHeader/TLemRunHeader.pc \
                 src/external/MuSR_software/Makefile \
                 src/external/MuSR_software/Class_MuSR_PSI/Makefile \
                 src/external/MuSR_software/Class_MuSR_PSI/Class_MuSR_PSI.pc \
                 src/external/mud/Makefile \
                 src/external/mud/src/Makefile \
                 src/external/mud/src/mud.pc \
                 src/external/libCuba/Makefile \
                 src/external/libCuba/src/Makefile \
                 src/external/libCuba/src/cuba.pc \
                 src/external/TFitPofB-lib/Makefile \
                 src/external/TFitPofB-lib/classes/Makefile \
                 src/external/libLFRelaxation/Makefile \
                 src/external/libGapIntegrals/Makefile \
                 src/external/libCalcMeanFieldsLEM/Makefile \
                 src/external/Nonlocal/Makefile])

AC_OUTPUT
