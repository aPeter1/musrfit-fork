AC_PREREQ(2.59)
AC_INIT(musrfit, 0.5.0, andreas.suter@psi.ch)
AC_CONFIG_AUX_DIR(admin)

dnl -----------------------------------------------
dnl Package names and version numbers
dnl -----------------------------------------------

MUSR_PROGRAM_NAME=musrfit
MUSR_LIBRARY_NAME=PMusr
LEM_LIBRARY_NAME=TLemRunHeader

#release versioning
MUSR_MAJOR_VERSION=0
MUSR_MINOR_VERSION=5
MUSR_MICRO_VERSION=0

#release versioning
LEM_MAJOR_VERSION=1
LEM_MINOR_VERSION=5
LEM_MICRO_VERSION=0

#API version
MUSR_API_VERSION=MUSR_MAJOR_VERSION.MUSR_MINOR_VERSION
AC_SUBST(MUSR_API_VERSION)

LEM_API_VERSION=LEM_MAJOR_VERSION.LEM_MINOR_VERSION
AC_SUBST(LEM_API_VERSION)

#shared library versioning
LEM_LIBRARY_VERSION=1:5:0
MUSR_LIBRARY_VERSION=0:5:0
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

PACKAGE=$MUSR_PROGRAM_NAME
AC_SUBST(MUSR_LIBRARY_NAME)
AC_SUBST(LEM_LIBRARY_NAME)

LEM_VERSION=$LEM_MAJOR_VERSION.$LEM_MINOR_VERSION.$LEM_MICRO_VERSION
LEM_RELEASE=$LEM_MAJOR_VERSION.$LEM_MINOR_VERSION
AC_SUBST(LEM_RELEASE)
AC_SUBST(LEM_VERSION)

MUSR_VERSION=$MUSR_MAJOR_VERSION.$MUSR_MINOR_VERSION.$MUSR_MICRO_VERSION
MUSR_RELEASE=$MUSR_MAJOR_VERSION.$MUSR_MINOR_VERSION
AC_SUBST(MUSR_RELEASE)
AC_SUBST(MUSR_VERSION)

VERSION=$MUSR_VERSION

dnl -----------------------------------------------
dnl Automake initialization and program checks
dnl -----------------------------------------------

AM_INIT_AUTOMAKE(@PACKAGE_NAME@, @PACKAGE_VERSION@)
AC_CONFIG_HEADER([config.h])
AC_LANG(C++)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LEX
AC_PROG_LN_S
AC_PROG_YACC
AC_PROG_INSTALL
AM_PROG_LIBTOOL
ROOT_PATH

dnl -----------------------------------------------
dnl Ask user for path to fftw3 stuff
dnl -----------------------------------------------

FFTW3_FOUND=0
AC_ARG_WITH([fftw3],
    [AC_HELP_STRING([--with-fftw3],[prefix of FFTW3 installation. e.g. /usr/local or /usr])],
    [FFTW3_PREFIX=$with_fftw3
     if !(test -r ${FFTW3_PREFIX}/include/fftw3.h); then
       AC_MSG_ERROR(FFTW3 cannot be found at the specified path!)
     fi
     AC_MSG_RESULT([using ${FFTW3_PREFIX} as prefix of the FFTW3 installation...])],
    [PKG_CHECK_MODULES(FFTW3, fftw3 >= 3.1.0, [FFTW3_FOUND=1],
     [if test -r /usr/local/include/fftw3.h; then
        FFTW3_PREFIX="/usr/local"
        AC_MSG_RESULT([using ${FFTW3_PREFIX} as prefix of the FFTW3 installation... (ignore the no just above)])
      elif test -r /usr/include/fftw3.h; then
        FFTW3_PREFIX="/usr"
        AC_MSG_RESULT([using ${FFTW3_PREFIX} as prefix of the FFTW3 installation... (ignore the no just above)])
      elif test -r /sw/include/fftw3.h; then
        FFTW3_PREFIX="/sw"
        AC_MSG_RESULT([using ${FFTW3_PREFIX} as prefix of the FFTW3 installation... (ignore the no just above)])
      else
        AC_MSG_ERROR(
        [FFTW3 not found. Please call configure with the --with-fftw3 option.
        This tells configure where to find the FFTW3 C libraries and headers.
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
AC_SUBST(FFTW3_LIBS)
AC_SUBST(FFTW3_CFLAGS)


dnl -----------------------------------------------
dnl Ask user for path to gsl stuff
dnl -----------------------------------------------

GSL_FOUND=0
AC_ARG_WITH([gsl],
    [AC_HELP_STRING([--with-gsl],[prefix of GSL installation. e.g. /usr/local or /usr])],
    [GSL_PREFIX=$with_gsl
     if !(test -r ${GSL_PREFIX}/include/gsl/gsl_sf_hyperg.h); then
       AC_MSG_ERROR(GSL cannot be found at the specified path!)
     fi
     AC_MSG_RESULT([using ${GSL_PREFIX} as prefix of the GSL installation...])],
    [PKG_CHECK_MODULES(GSL, gsl >= 1.9, [GSL_FOUND=1],
     [if test -r /usr/local/include/gsl/gsl_sf_hyperg.h; then
        GSL_PREFIX="/usr/local"
        AC_MSG_RESULT([using ${GSL_PREFIX} as prefix of the GSL installation... (ignore the no just above)])
      elif test -r /usr/include/gsl/gsl_sf_hyperg.h; then
        GSL_PREFIX="/usr"
        AC_MSG_RESULT([using ${GSL_PREFIX} as prefix of the GSL installation... (ignore the no just above)])
      elif test -r /sw/include/gsl/gsl_sf_hyperg.h; then
        GSL_PREFIX="/sw"
        AC_MSG_RESULT([using ${GSL_PREFIX} as prefix of the GSL installation... (ignore the no just above)])
      else
        AC_MSG_ERROR(
        [GSL not found. Please call configure with the --with-gsl option.
        This tells configure where to find the GSL C libraries and headers.
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
dnl Ask user for path to boost stuff
dnl -----------------------------------------------

BOOST_FOUND=0
AC_ARG_WITH([boostinc],
    [AC_HELP_STRING([--with-boostinc],[path to the header files of the Boost installation, e.g. /usr/local/include or /usr/include])],
    [BOOST_INCLUDE=$with_boostinc
     if !(test -r ${BOOST_INCLUDE}/boost/spirit.hpp); then
       AC_MSG_ERROR(Boost cannot be found at the specified path!)
     fi
     AC_MSG_RESULT([using ${BOOST_INCLUDE} as include path for the Boost headers...])],
    [PKG_CHECK_MODULES(BOOST, boost >= 1.30, [BOOST_FOUND=1],
     [if test -r /usr/local/include/boost/spirit.hpp; then
        BOOST_INCLUDE="/usr/local/include"
        AC_MSG_RESULT([using ${BOOST_INCLUDE} as include path for the Boost headers... (ignore the no just above)])
      elif test -r /usr/include/boost/spirit.hpp; then
        BOOST_INCLUDE="/usr/include"
        AC_MSG_RESULT([using ${BOOST_INCLUDE} as include path for the Boost headers... (ignore the no just above)])
      elif test -r /sw/include/boost/spirit.hpp; then
        BOOST_INCLUDE="/sw/include"
        AC_MSG_RESULT([using ${BOOST_INCLUDE} as include path for the Boost headers... (ignore the no just above)])
      else
        AC_MSG_ERROR(
        [Boost not found. Please call configure with the --with-boostinc option.
        This tells configure where to find the Boost C++ headers.
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

ROOT_LIBS="-L@ROOTLIBDIR@ @ROOTGLIBS@ -lMinuit2 -lMathMore -lXMLParser"
ROOT_CFLAGS="-I@ROOTINCDIR@ @ROOTCFLAGS@"
AC_SUBST(ROOT_LIBS)
AC_SUBST(ROOT_CFLAGS)


dnl -----------------------------------------------
dnl Set some paths and flags for PMusr and TLemRunHeader
dnl -----------------------------------------------

SRCDIR="$(pwd)"

LEM_SRCDIR="${SRCDIR}/src/external/TLemRunHeader"
LEM_LIBS="-L${LEM_SRCDIR} -l${LEM_LIBRARY_NAME}"
LEM_CFLAGS="-I${LEM_SRCDIR}"
AC_SUBST(LEM_LIBS)
AC_SUBST(LEM_CFLAGS)

PMUSR_LIBS="-L${SRCDIR}/src/classes -l${MUSR_LIBRARY_NAME}"
PMUSR_CFLAGS="-I${SRCDIR}/src/include"
AC_SUBST(PMUSR_LIBS)
AC_SUBST(PMUSR_CFLAGS)


dnl -----------------------------------------------
dnl Set host specific compiler and linker flags
dnl -----------------------------------------------

LOCAL_BIN_CXXFLAGS="-Wall -Wno-trigraphs"
LOCAL_LIB_CXXFLAGS="${LOCAL_BIN_CXXFLAGS}"
LOCAL_BIN_LDFLAGS=
LOCAL_LIB_LDFLAGS=

case "$host" in
  *-*-cygwin)
    LOCAL_BIN_CXXFLAGS="${LOCAL_BIN_CXXFLAGS}"
    LOCAL_LIB_CXXFLAGS="${LOCAL_BIN_CXXFLAGS} -D_DLL"
    LOCAL_BIN_LDFLAGS="${LOCAL_BIN_LDFLAGS} -Wl,--enable-auto-import -Wl,--enable-runtime-pseudo-reloc"
    LOCAL_LIB_LDFLAGS="${LOCAL_BIN_LDFLAGS} -Wl,--export-all-symbols"
    ;;
esac

AC_SUBST(LOCAL_BIN_CXXFLAGS)
AC_SUBST(LOCAL_LIB_CXXFLAGS)
AC_SUBST(LOCAL_BIN_LDFLAGS)
AC_SUBST(LOCAL_LIB_LDFLAGS)


dnl -----------------------------------------------
dnl Some standard checks for header files and libraries
dnl -----------------------------------------------

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([floor gettimeofday memset pow sqrt strstr])


dnl -----------------------------------------------
dnl Specify the files that are going to be created by configure
dnl -----------------------------------------------

AC_CONFIG_FILES([Makefile \
                 src/Makefile \
                 src/classes/Makefile \
                 src/classes/PMusr.pc \
                 src/external/Makefile \
                 src/external/TLemRunHeader/Makefile \
                 src/external/TLemRunHeader/TLemRunHeader.pc \
                 src/external/scripts/Makefile])
AC_OUTPUT
