AC_REVISION([$Id$])

AC_PREREQ(2.59)
AC_INIT(musrfit, 0.9.0, andreas.suter@psi.ch)
AC_CONFIG_AUX_DIR(admin)
AC_CANONICAL_HOST
#AC_MSG_RESULT([${host} ${host_cpu} ${host_vendor} ${host_os}])

dnl -----------------------------------------------
dnl Check if pkg-config is installed
dnl -----------------------------------------------

#PKG_PROG_PKG_CONFIG([0.9])
#The above macro does not work if pkg-config is not installed, therefore, for the moment we stay with the solution below:
m4_ifdef([PKG_CHECK_MODULES],[],AC_MSG_ERROR([Please install pkg-config before configuring musrfit!]))

dnl -----------------------------------------------
dnl Save the configure command
dnl -----------------------------------------------
CONFIGCOMMAND="$0 $@"

dnl -----------------------------------------------
dnl Package names and version numbers
dnl -----------------------------------------------

MUSR_PROGRAM_NAME=musrfit
MUSR_LIBRARY_NAME=PMusr
USERFCN_LIBRARY_NAME=PUserFcnBase
LEM_LIBRARY_NAME=TLemRunHeader
PSIBIN_LIBRARY_NAME=Class_MuSR_PSI
MUD_LIBRARY_NAME=mud
PNEXUS_LIBRARY_NAME=PNeXus
CUBA_LIBRARY_NAME=cuba

dnl -----------------------------------------------
dnl Optional plug-ins
dnl -----------------------------------------------


#release versioning
MUSR_MAJOR_VERSION=0
MUSR_MINOR_VERSION=9
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
PNEXUS_MAJOR_VERSION=0
PNEXUS_MINOR_VERSION=9
PNEXUS_MICRO_VERSION=0

#release versioning
PLUGIN_MAJOR_VERSION=1
PLUGIN_MINOR_VERSION=0
PLUGIN_MICRO_VERSION=0

#release versioning
CUBA_MAJOR_VERSION=2
CUBA_MINOR_VERSION=1
CUBA_MICRO_VERSION=0

#API version
MUSR_API_VERSION=$MUSR_MAJOR_VERSION.$MUSR_MINOR_VERSION
AC_SUBST(MUSR_API_VERSION)

LEM_API_VERSION=$LEM_MAJOR_VERSION.$LEM_MINOR_VERSION
AC_SUBST(LEM_API_VERSION)

PSIBIN_API_VERSION=$PSIBIN_MAJOR_VERSION.$PSIBIN_MINOR_VERSION
AC_SUBST(PSIBIN_API_VERSION)

MUD_API_VERSION=$MUD_MAJOR_VERSION.$MUD_MINOR_VERSION
AC_SUBST(MUD_API_VERSION)

PNEXUS_API_VERSION=$PNEXUS_MAJOR_VERSION.$PNEXUS_MINOR_VERSION
AC_SUBST(PNEXUS_API_VERSION)

PLUGIN_API_VERSION=$PLUGIN_MAJOR_VERSION.$PLUGIN_MINOR_VERSION
AC_SUBST(PLUGIN_API_VERSION)

CUBA_API_VERSION=$CUBA_MAJOR_VERSION.$CUBA_MINOR_VERSION
AC_SUBST(CUBA_API_VERSION)

#shared library versioning
CUBA_LIBRARY_VERSION=$CUBA_MAJOR_VERSION:$CUBA_MINOR_VERSION:$CUBA_MICRO_VERSION
PLUGIN_LIBRARY_VERSION=$PLUGIN_MAJOR_VERSION:$PLUGIN_MINOR_VERSION:$PLUGIN_MICRO_VERSION
LEM_LIBRARY_VERSION=$LEM_MAJOR_VERSION:$LEM_MINOR_VERSION:$LEM_MICRO_VERSION
PSIBIN_LIBRARY_VERSION=$PSIBIN_MAJOR_VERSION:$PSIBIN_MINOR_VERSION:$PSIBIN_MICRO_VERSION
MUD_LIBRARY_VERSION=$MUD_MAJOR_VERSION:$MUD_MINOR_VERSION:$MUD_MICRO_VERSION
PNEXUS_LIBRARY_VERSION=$PNEXUS_MAJOR_VERSION:$PNEXUS_MINOR_VERSION:$PNEXUS_MICRO_VERSION
MUSR_LIBRARY_VERSION=$MUSR_MAJOR_VERSION:$MUSR_MINOR_VERSION:$MUSR_MICRO_VERSION

# This is definitely handled wrongly at the moment and needs to be fixed...  
#XXX_LIBRARY_VERSION=X:Y:Z
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
AC_SUBST(PNEXUS_LIBRARY_VERSION)
AC_SUBST(PLUGIN_LIBRARY_VERSION)
AC_SUBST(CUBA_LIBRARY_VERSION)

PACKAGE=$MUSR_PROGRAM_NAME
AC_SUBST(MUSR_LIBRARY_NAME)
AC_SUBST(USERFCN_LIBRARY_NAME)
AC_SUBST(LEM_LIBRARY_NAME)
AC_SUBST(PSIBIN_LIBRARY_NAME)
AC_SUBST(MUD_LIBRARY_NAME)
AC_SUBST(PNEXUS_LIBRARY_NAME)
AC_SUBST(CUBA_LIBRARY_NAME)

PSIBIN_VERSION=$PSIBIN_MAJOR_VERSION.$PSIBIN_MINOR_VERSION.$PSIBIN_MICRO_VERSION
PSIBIN_RELEASE=$PSIBIN_MAJOR_VERSION.$PSIBIN_MINOR_VERSION
AC_SUBST(PSIBIN_RELEASE)
AC_SUBST(PSIBIN_VERSION)

MUD_VERSION=$MUD_MAJOR_VERSION.$MUD_MINOR_VERSION.$MUD_MICRO_VERSION
MUD_RELEASE=$MUD_MAJOR_VERSION.$MUD_MINOR_VERSION
AC_SUBST(MUD_RELEASE)
AC_SUBST(MUD_VERSION)

PNEXUS_VERSION=$PNEXUS_MAJOR_VERSION.$PNEXUS_MINOR_VERSION.$PNEXUS_MICRO_VERSION
PNEXUS_RELEASE=$PNEXUS_MAJOR_VERSION.$PNEXUS_MINOR_VERSION
AC_SUBST(PNEXUS_RELEASE)
AC_SUBST(PNEXUS_VERSION)

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

# Wordaround for gcc-bugs causing problems building libCuba
# which should be change when a real solution appears
# Set CFLAGS to "" and pass any user-CFLAGS to the mud-building
# user-CFLAGS will not be passed to the libCuba building process
# mud and cuba are currently the only C-routines
USER_CFLAGS="$CFLAGS"
CFLAGS=
if test "$USER_CFLAGS" = ""; then
  USER_CFLAGS="-g -O2"
fi

dnl -----------------------------------------------
dnl Automake initialization and program checks
dnl -----------------------------------------------

AM_INIT_AUTOMAKE(@PACKAGE_NAME@, @PACKAGE_VERSION@)
m4_ifdef([AM_SILENT_RULES],
         [AM_SILENT_RULES([yes])])
AC_CONFIG_HEADER([config.h])
AC_LANG([C++])
AC_PROG_LN_S
AC_PROG_INSTALL

m4_ifdef([AC_PROG_MKDIR_P],
         [AC_PROG_MKDIR_P],
         [MKDIR_P="mkdir -p"
          AC_SUBST(MKDIR_P)])

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
if test "x$(${ROOTCONF} --has-minuit2)" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([Minuit2 is not included in the ROOT installation... Please configure ROOT with the --enable-minuit2 option!])
fi

AC_MSG_CHECKING([for MathMore in ROOT])
if test "x$(${ROOTCONF} --has-mathmore)" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([MathMore is not included in the ROOT installation... Please configure ROOT with the --enable-mathmore option and check that the feature is actually built!])
fi

AC_MSG_CHECKING([for XML support in ROOT])
if test "x$(${ROOTCONF} --has-xml)" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([XML support is not included in the ROOT installation... Please configure ROOT with the --enable-xml option and check that the feature is actually built!])
fi

AC_MSG_CHECKING([for image processing support in ROOT])
if test "x$(${ROOTCONF} --has-asimage)" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([image processing support is not included in the ROOT installation... Please configure ROOT with the --enable-asimage option and check that the feature is actually built!])
fi

AC_SUBST(ROOT_LIBS)
AC_SUBST(ROOT_CFLAGS)

dnl -----------------------------------------------
dnl Check for NeXus, HDF5, etc.
dnl -----------------------------------------------
AC_ARG_ENABLE([NeXus], [AC_HELP_STRING([--enable-NeXus], [build optional NeXus support [default=no]])],
    [
    # checking for hdf4
    AC_ARG_WITH([hdf4],
        [AC_HELP_STRING([--with-hdf4], [prefix of the HDF4 installation (needed for NeXus), e.g. /usr/local/hdf4 or /opt/hdf4])],
        [HDF4_PREFIX=$with_hdf4
         AC_MSG_CHECKING([whether HDF4 can be found at the specified location])
         if !(test -r ${HDF4_PREFIX}/include/hdf.h); then
           AC_MSG_RESULT([no])
           AC_MSG_ERROR([HDF4 cannot be found at the specified path!])
         fi
         AC_MSG_RESULT([${HDF4_PREFIX}])],
        [AC_MSG_CHECKING([whether HDF4 is installed in a standard location])
         if test -r /usr/local/include/hdf.h; then
            HDF4_PREFIX="/usr/local"
            AC_MSG_RESULT([${HDF4_PREFIX}])
         elif test -r /usr/local/hdf/include/hdf.h; then
           HDF4_PREFIX="/usr/local/hdf"
           AC_MSG_RESULT([${HDF4_PREFIX}])
         elif test -r /usr/include/hdf.h; then
           HDF4_PREFIX="/usr"
           AC_MSG_RESULT([${HDF4_PREFIX}])
         elif test -r /usr/include/hdf/hdf.h; then
           HDF4_PREFIX="/usr/include/"
           AC_MSG_RESULT([${HDF4_PREFIX}])
         elif test -r /sw/include/hdf.h; then
           HDF4_PREFIX="/sw"
           AC_MSG_RESULT([${HDF4_PREFIX}])
         elif test -r /opt/local/include/hdf.h; then
           HDF4_PREFIX="/opt/local"
           AC_MSG_RESULT([${HDF4_PREFIX}])
         else
           AC_MSG_RESULT([no])
           AC_MSG_ERROR(
           [HDF4 not found (needed for NeXus). Please call configure with the --with-hdf4 option.
           This tells configure where to find the HDF4 C++ headers,
           e.g. --with-hdf4=/usr/local/hdf]
           )
         fi 
        ]
    )

    # checking for hdf5
    AC_ARG_WITH([hdf5],
        [AC_HELP_STRING([--with-hdf5], [prefix of the HDF5 installation (needed for NeXus), e.g. /usr/local/hdf5 or /opt/hdf5])],
        [HDF5_PREFIX=$with_hdf5
         AC_MSG_CHECKING([whether HDF5 can be found at the specified location])
         if !(test -r ${HDF5_PREFIX}/include/hdf5.h); then
           AC_MSG_RESULT([no])
           AC_MSG_ERROR([HDF5 cannot be found at the specified path!])
         fi
         AC_MSG_RESULT([${HDF5_PREFIX}])],
        [AC_MSG_CHECKING([whether HDF5 is installed in a standard location])
         if test -r /usr/local/include/hdf5.h; then
            HDF5_PREFIX="/usr/local"
            AC_MSG_RESULT([${HDF5_PREFIX}])
         elif test -r /usr/local/hdf5/include/hdf5.h; then
           HDF5_PREFIX="/usr/local/hdf5"
           AC_MSG_RESULT([${HDF5_PREFIX}])
         elif test -r /usr/include/hdf5.h; then
           HDF5_PREFIX="/usr"
           AC_MSG_RESULT([${HDF5_PREFIX}])
         elif test -r /sw/include/hdf5.h; then
           HDF5_PREFIX="/sw"
           AC_MSG_RESULT([${HDF5_PREFIX}])
         elif test -r /opt/local/include/hdf5.h; then
           HDF5_PREFIX="/opt/local"
           AC_MSG_RESULT([${HDF5_PREFIX}])
         else
           AC_MSG_RESULT([no])
           AC_MSG_ERROR(
           [HDF5 not found (needed for NeXus). Please call configure with the --with-hdf5 option.
           This tells configure where to find the HDF5 C++ headers,
           e.g. --with-hdf5=/usr/local/hdf5]
           )
         fi 
        ]
    )

    # checking for NeXus
    NEXUS_FOUND=0
    AC_ARG_WITH([nexus],
        [AC_HELP_STRING([--with-nexus], [prefix of the NeXus installation, e.g. /usr/local])],
        [NEXUS_PREFIX=$with_nexus
         AC_MSG_CHECKING([whether NeXus can be found at the specified location])
         if !(test -r ${NEXUS_PREFIX}/include/napi.h); then
           AC_MSG_RESULT([no])
           AC_MSG_ERROR([NeXus cannot be found at the specified path!])
         fi
         AC_MSG_RESULT([${NEXUS_PREFIX}])],
        [PKG_CHECK_MODULES(NEXUS, nexus >= 4.1, [NEXUS_FOUND=1],
         [AC_MSG_CHECKING([whether NeXus is installed in a standard location])
          if test -r /usr/local/include/napi.h; then
            NEXUS_PREFIX="/usr/local"
            AC_MSG_RESULT([${NEXUS_PREFIX}])
         elif test -r /usr/local/nexus/include/napi.h; then
           NEXUS_PREFIX="/usr/local/nexus"
           AC_MSG_RESULT([${NEXUS_PREFIX}])
         elif test -r /usr/include/napi.h; then
           NEXUS_PREFIX="/usr"
           AC_MSG_RESULT([${NEXUS_PREFIX}])
         elif test -r /sw/include/napi.h; then
           NEXUS_PREFIX="/sw"
           AC_MSG_RESULT([${NEXUS_PREFIX}])
         elif test -r /opt/local/include/napi.h; then
           NEXUS_PREFIX="/opt/local"
           AC_MSG_RESULT([${NEXUS_PREFIX}])
         else
           AC_MSG_RESULT([no])
           AC_MSG_ERROR(
           [NeXus not found. Please call configure with the --with-nexus option.
           This tells configure where to find the NeXus headers,
           e.g. --with-nexus=/usr/local/nexus]
           )
         fi
        ]
       )]
    )
    AC_SUBST(NEXUS_PREFIX)
    if test "${NEXUS_FOUND}" != "1"; then
      NEXUS_LIBS="-L${NEXUS_PREFIX}/lib -lNeXus"
      NEXUS_CFLAGS="-I${NEXUS_PREFIX}/include"
    fi
    AC_SUBST(NEXUS_LIBS)
    AC_SUBST(NEXUS_CFLAGS)

    PNEXUS_ENABLED=1
    ],
    [PNEXUS_ENABLED=0]
)

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
      AS_IF([test "x$GCC" = "xyes"],
        [case "$($CC --version 2>&1 < /dev/null)" in
          *gcc*4.2* | *gcc*4.4.3*)
            opt=-O0
            ;;
          *)
            opt=-O3
            ;;
        esac
        CUBA_BUILD_CFLAGS="$opt -fomit-frame-pointer -ffast-math"],
        [CUBA_BUILD_CFLAGS="-O"])

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
dnl Set some paths and flags for PMusr, TLemRunHeader, Class_MuSR_PSI, mud, NeXus (if enabled), etc.
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

USERFCN_LIBS="${SRCDIR}/classes/lib${USERFCN_LIBRARY_NAME}.la"
AC_SUBST(USERFCN_LIBS)

if test "${PNEXUS_ENABLED}" = "1"; then
  AC_DEFINE([PNEXUS_ENABLED], [1], [Define to 1 if NeXus is enabled])
  PNEXUS_SRCDIR="${SRCDIR}/external/nexus"
  PNEXUS_LIBS="${PNEXUS_SRCDIR}/lib${PNEXUS_LIBRARY_NAME}.la"
  PNEXUS_CXXFLAGS="-I${PNEXUS_SRCDIR}"
  AC_SUBST(PNEXUS_LIBS)
  AC_SUBST(PNEXUS_CXXFLAGS)
fi

if test "${BUILD_BMW_LIBS}" = "1"; then
  BMWTOOLS_SRCDIR="${SRCDIR}/external/BMWtools"
  BMWTOOLS_LIBS="${BMWTOOLS_SRCDIR}/libBMWtools.la"
  BMWTOOLS_CFLAGS="-I${BMWTOOLS_SRCDIR}"
  AC_SUBST(BMWTOOLS_LIBS)
  AC_SUBST(BMWTOOLS_CFLAGS)

  FITPOFB_SRCDIR="${SRCDIR}/external/libFitPofB"
  FITPOFB_LIBS="${FITPOFB_SRCDIR}/classes/libFitPofB.la"
  FITPOFB_CFLAGS="-I${FITPOFB_SRCDIR}/include"
  AC_SUBST(FITPOFB_LIBS)
  AC_SUBST(FITPOFB_CFLAGS)

  # Check for fftw3_threads-library. If available musrfit is also linked against it (used in libFitPofB).
  SAVED_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $FFTW3_CFLAGS"
  SAVED_LIBSS="$LIBS"
  LIBS="$LIBS $FFTW3_LIBS"
  AC_SEARCH_LIBS([fftw_init_threads], [fftw3_threads], [FFTW3_LIBS="$FFTW3_LIBS -lfftw3_threads -lpthread"
                                      AC_DEFINE([HAVE_LIBFFTW3_THREADS], [1], [Define to 1 if fftw3_threads are available])], [], [-lpthread])
  # Check for fftw3f library. If it is not available the BMWlibs will not be built!
  AC_SEARCH_LIBS([fftwf_malloc], [fftw3f], [FFTW3_LIBS="$FFTW3_LIBS -lfftw3f"], [BUILD_BMW_LIBS=0
    AC_MSG_ERROR([The float version of FFTW3 is not available. When building FFTW3 from source it should be configured with the options
                  --enable-shared --enable-float. Without this library the BMWlibs cannot be built!])], [])
  if test "${BUILD_BMW_LIBS}" = "1"; then
    AC_SEARCH_LIBS([fftwf_init_threads], [fftw3f_threads], [FFTW3_LIBS="$FFTW3_LIBS -lfftw3f_threads -lpthread"
                                      AC_DEFINE([HAVE_LIBFFTW3F_THREADS], [1], [Define to 1 if fftw3f_threads are available])], [], [-lpthread])
  fi

  CFLAGS="$SAVED_CFLAGS"
  LIBS="$SAVED_LIBS"
fi

AC_SUBST(FFTW3_LIBS)
AC_SUBST(FFTW3_CFLAGS)

dnl -----------------------------------------------
dnl Ask user if OpenMP support should be disabled (used for parallel chisq calculation and in libFitPofB)
dnl -----------------------------------------------

AC_ARG_ENABLE([omp], [AC_HELP_STRING([--enable-omp],[build musrfit with OpenMP support [default=yes]])])

if test "x$enable_omp" != "xno"; then
  SAVED_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -fopenmp"
  SAVED_LIBSS="$LIBS"
  LIBS="$LIBS -fopenmp -lgomp"
  AC_SEARCH_LIBS([omp_get_num_procs], [gomp], [AC_DEFINE([HAVE_GOMP], [1], [Define to 1 if gomp is available])],
                                      [CXXFLAGS="$SAVED_CXXFLAGS" LIBS="$SAVED_LIBS"], [])
fi

dnl -----------------------------------------------
dnl Ask user if the building of musredit/musrgui should be disabled
dnl -----------------------------------------------

AC_ARG_ENABLE([editor], [AC_HELP_STRING([--enable-editor],[check for Qt and build musredit or musrgui [default=yes]])])

if test "x$enable_editor" != "xno"; then

  echo "Additional checks for musredit/musrgui ..."

  case "$host" in
    *-*-cygwin)
      QT4MINVER=4.5
      ;;
    *)
      QT4MINVER=4.6
      ;;
  esac

  QT3_FOUND=no
  QT3MT_FOUND=no
  QT4_FOUND=no
  QMAKEBIN=""
  QTEDITOR=none

  AC_ARG_WITH([qt3],
        [AC_HELP_STRING([--with-qt3],[prefix of the Qt3 installation, e.g. /usr/lib/qt-3.3])],
        [QT3_PREFIX=$with_qt3
        AC_MSG_CHECKING([whether qmake for Qt3 can be found at the specified location])
        for QMAKE in qmake qmake-qt3
        do
          if test -x "${QT3_PREFIX}/bin/${QMAKE}"; then
            QMAKEVERSIONSTRING="$(${QT3_PREFIX}/bin/${QMAKE} -v 2>&1)"
            for a in $QMAKEVERSIONSTRING
            do
              if test "$(expr match "$a" '\(..\)')" = "3."; then
                QMAKEBIN="${QT3_PREFIX}/bin/${QMAKE}"
                QTEDITOR=musrgui
                AC_MSG_RESULT([${QMAKEBIN}])
                break
              fi
            done
            if test "x${QMAKEBIN}" != "x"; then
              break
            fi
          fi
        done

        if test "x${QMAKEBIN}" = "x"; then
          AC_MSG_RESULT([no])
          AC_MSG_ERROR([qmake for Qt3 cannot be found in ${QT3_PREFIX}/bin!])
        fi]
  )

  AC_ARG_WITH([qt4],
        [AC_HELP_STRING([--with-qt4],[prefix of the Qt4 installation, e.g. /opt/qt-4.7])],
        [QT4_PREFIX=$with_qt4
        AC_MSG_CHECKING([whether qmake for Qt4 can be found at the specified location])
        for QMAKE in qmake qmake-qt4
        do
          if test -x "${QT4_PREFIX}/bin/${QMAKE}"; then
            QMAKEVERSIONSTRING="$(${QT4_PREFIX}/bin/${QMAKE} -v 2>&1)"
            for a in $QMAKEVERSIONSTRING
            do
              if test "$(expr match "$a" '\(..\)')" = "4."; then
                QMAKEBIN="${QT4_PREFIX}/bin/${QMAKE}"
                QTEDITOR=musredit
                AC_MSG_RESULT([${QMAKEBIN}])
                break
              fi
            done
            if test "x${QMAKEBIN}" != "x"; then
              break
            fi
          fi
        done

        if test "x${QMAKEBIN}" = "x"; then
          AC_MSG_RESULT([no])
          AC_MSG_ERROR([qmake for Qt4 cannot be found in ${QT4_PREFIX}/bin!])
        fi

        if test "x${QT3PRESENT}" = "xyes"; then
          AC_MSG_WARN([Qt3 and Qt4 directories have been given -- only the Qt4 information will be used and only musredit will be built!])
        fi],
        [if test "x${QMAKEBIN}" = "x"; then
          PKG_CHECK_MODULES(QT4, QtWebKit >= $QT4MINVER QtXml >= $QT4MINVER, [QT4_FOUND=yes],
            [PKG_CHECK_MODULES(QT3, qt >= 3.3.0 qt < 4.0, [QT3_FOUND=yes],
              [PKG_CHECK_MODULES(QT3MT, qt-mt >= 3.3.0 qt-mt < 4.0, [QT3MT_FOUND=yes], [QT3MT_FOUND=no])]
            )]
          )
        fi]
  )

  if test "x${QMAKEBIN}" = "x"; then
    if test "x${QT3_FOUND}" = "xno" && test "x${QT3MT_FOUND}" = "xno" && test "x${QT4_FOUND}" = "xno"; then
      AC_MSG_WARN([No suitable version of either Qt4 or Qt3 has been found! musredit/musrgui will not be built in this stage!])
    else
      # Check for qmake on the basis of the pkg-config information
      QTPATH=none

      if test "x$QT4_FOUND" = "xyes"; then
        QTPATH=$(pkg-config --variable=prefix QtWebKit)
        QTEDITOR=musredit
      elif test "x$QT3_FOUND" = "xyes"; then
        QTPATH=$(pkg-config --variable=prefix qt)
        QTEDITOR=musrgui
      elif test "x$QT3MT_FOUND" = "xyes"; then
        QTPATH=$(pkg-config --variable=prefix qt-mt)
        QTEDITOR=musrgui
      fi

      AC_MSG_CHECKING([for qmake])
      if test "x${QTPATH}" != "xnone"; then
        if test "x$QTEDITOR" = "xmusredit"; then
          for QMAKE in qmake-qt4 qmake-qt46 qmake-qt47 qmake-qt48
          do
            if test -x "${QTPATH}/bin/${QMAKE}"; then
              QMAKEBIN="${QTPATH}/bin/${QMAKE}"
              AC_MSG_RESULT([${QMAKEBIN}])
              break
            fi
          done
          if test "x${QMAKEBIN}" = "x"; then
            if test -x "${QTPATH}/bin/qmake"; then
              QMAKEVERSIONSTRING="$(${QTPATH}/bin/qmake -v 2>&1)"
              for a in $QMAKEVERSIONSTRING
              do
                if test "$(expr match "$a" '\(..\)')" = "4."; then
                  QMAKEBIN="${QTPATH}/bin/qmake"
                  AC_MSG_RESULT([${QMAKEBIN}])
                  break
                fi
              done
            fi
          fi
        elif test "x$QTEDITOR" = "xmusrgui"; then
          for QMAKE in qmake-qt3 qmake-qt33
          do
            if test -x "${QTPATH}/bin/${QMAKE}"; then
              QMAKEBIN="${QTPATH}/bin/${QMAKE}"
              AC_MSG_RESULT([${QMAKEBIN}])
              break
            fi
          done
          if test "x${QMAKEBIN}" = "x"; then
            if test -x "${QTPATH}/bin/qmake"; then
              QMAKEVERSIONSTRING="$(${QTPATH}/bin/qmake -v 2>&1)"
              for a in $QMAKEVERSIONSTRING
              do
                if test "$(expr match "$a" '\(..\)')" = "3."; then
                  QMAKEBIN="${QTPATH}/bin/qmake"
                  AC_MSG_RESULT([${QMAKEBIN}])
                  break
                fi
              done
            fi
          fi
        fi
        if test "x${QMAKEBIN}" = "x"; then
          AC_MSG_RESULT([no])
          AC_MSG_WARN([If Qt4 or Qt3 are present but have not been detected, try using the --with-qt4 or --with-qt3 options!])
        fi
      fi
    fi
  fi
fi

AM_CONDITIONAL([BUILD_MUSREDIT], [test "x$enable_editor" != "xno" && test "x${QMAKEBIN}" != "x" && test "x${QTEDITOR}" = "xmusredit"])
AM_CONDITIONAL([BUILD_MUSRGUI], [test "x$enable_editor" != "xno" && test "x${QMAKEBIN}" != "x" && test "x${QTEDITOR}" = "xmusrgui"])

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
LOCAL_MUD_LIB_CFLAGS="${LOCAL_LIB_CXXFLAGS} ${USER_CFLAGS}"
LOCAL_PNEXUS_LIB_CXXFLAGS="${LOCAL_LIB_CXXFLAGS}"
LOCAL_CUBA_LIB_CFLAGS="${LOCAL_LIB_CXXFLAGS} ${CUBA_BUILD_CFLAGS}"
LOCAL_BIN_LDFLAGS=
LOCAL_LIB_LDFLAGS=

case "$host" in
  *-*-cygwin)
    ARCH=CYGWIN
    AC_DEFINE([_WIN32GCC], [1], [Define to 1 if host system is Cygwin])
    LOCAL_BIN_CXXFLAGS="${LOCAL_BIN_CXXFLAGS}"
    LOCAL_LIB_CXXFLAGS="${LOCAL_BIN_CXXFLAGS} -D_DLL"
    LOCAL_PSIBIN_LIB_CXXFLAGS="${LOCAL_LIB_CXXFLAGS} -D_WIN32GCC"
    LOCAL_MUD_LIB_CFLAGS="${LOCAL_LIB_CXXFLAGS} ${USER_CFLAGS}"
    LOCAL_PNEXUS_LIB_CXXFLAGS="${LOCAL_LIB_CXXFLAGS}"
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
AC_SUBST(LOCAL_MUD_LIB_CFLAGS)
AC_SUBST(LOCAL_PNEXUS_LIB_CXXFLAGS)
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
dnl Determine the installation directory for the documentation
dnl -----------------------------------------------

if test "x${prefix}" = "xNONE"
then
  INSTALLDIR="/usr/local"
else
  INSTALLDIR="${prefix}"
fi

if test -d "${INSTALLDIR}/doc"
then
  DOCDIR="${INSTALLDIR}/doc/musrfit"
else
  DOCDIR="${INSTALLDIR}/share/doc/musrfit"
fi

AC_SUBST(DOCDIR)

dnl -----------------------------------------------
dnl Specify the files that are going to be created by configure
dnl -----------------------------------------------

AM_CONDITIONAL([IS_DARWIN], [test "${ARCH}" = "DARWIN"])
AM_CONDITIONAL([IS_CYGWIN], [test "${ARCH}" = "CYGWIN"])

AM_CONDITIONAL([PNEXUS_ENABLED], [test "${PNEXUS_ENABLED}" = "1"])
AM_CONDITIONAL([BUILD_CUBALIB], [test "${BUILD_CUBA}" = "1"])
AM_CONDITIONAL([BUILD_BMWLIBS], [test "${BUILD_BMW_LIBS}" = "1"])
AM_CONDITIONAL([BUILD_ASLIBS], [test "${BUILD_AS_LIBS}" = "1"])

AC_CONFIG_FILES([Makefile \
                 src/Makefile \
                 src/classes/Makefile \
                 src/classes/PMusr.pc \
                 src/classes/PUserFcnBase.pc \
                 src/external/Makefile \
                 src/external/TLemRunHeader/Makefile \
                 src/external/TLemRunHeader/TLemRunHeader.pc \
                 src/external/MuSR_software/Makefile \
                 src/external/MuSR_software/Class_MuSR_PSI/Makefile \
                 src/external/MuSR_software/Class_MuSR_PSI/Class_MuSR_PSI.pc \
                 src/external/mud/Makefile \
                 src/external/mud/src/Makefile \
                 src/external/mud/src/mud.pc \
                 src/external/nexus/Makefile \
                 src/external/nexus/PNeXus.pc \
                 src/external/libCuba/Makefile \
                 src/external/libCuba/src/Makefile \
                 src/external/libCuba/src/cuba.pc \
                 src/external/BMWtools/Makefile \
                 src/external/libFitPofB/Makefile \
                 src/external/libFitPofB/classes/Makefile \
                 src/external/libLFRelaxation/Makefile \
                 src/external/libZFRelaxation/Makefile \
                 src/external/libGapIntegrals/Makefile \
                 src/external/libCalcMeanFieldsLEM/Makefile \
                 src/external/Nonlocal/Makefile \
                 src/external/MagProximity/Makefile \
                 src/musredit/musredit_startup.xml \
                 src/musrgui/musrgui_startup.xml])

AC_OUTPUT

if test "x$enable_editor" != "xno" && test "x$QMAKEBIN" != "x"; then

  echo "Configuring ${QTEDITOR} ..."

  echo "Changing directory to src/${QTEDITOR} and calling"
  echo "${QMAKEBIN} CC=${CC} CXX=${CXX} PREFIX=${INSTALLDIR} ${QTEDITOR}.pro"
  cd src/${QTEDITOR}
  ${QMAKEBIN} CC=${CC} CXX=${CXX} PREFIX=${INSTALLDIR} ${QTEDITOR}.pro
  cd ../..

fi

echo ""
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "+                                                                           +"
echo "+                                  Summary                                  +"
echo "+                                                                           +"
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo ""

echo "   Invocation command line:"
echo "   ------------------------"
echo ""
echo "   $ ${CONFIGCOMMAND}"
echo ""
echo ""

echo "   System:"
echo "   -------"
echo ""
echo "   $host"
echo ""
echo ""
echo "   Requirements:"
echo "   -------------"
echo ""
if test "${FFTW3_FOUND}" = "1"; then
  echo "   FFTW3 found in $(pkg-config --variable=prefix fftw3)"
else
  echo "   FFTW3 found in ${FFTW3_PREFIX}"
fi
if test "${GSL_FOUND}" = "1"; then
  echo "   GSL found in $(pkg-config --variable=prefix gsl)"
else
  echo "   GSL found in ${GSL_PREFIX}"
fi
if test "${BOOST_FOUND}" = "1"; then
  echo "   BOOST header files found in $(pkg-config --variable=prefix boost)"
else
  echo "   BOOST header files found in ${BOOST_INCLUDE}"
fi
echo "   ROOT found in ${ROOTLIBDIR%/lib}"
echo ""
if test "${PNEXUS_ENABLED}" -eq 1; then
  echo "   HDF4 found in ${HDF4_PREFIX}"
  echo "   HDF5 found in ${HDF5_PREFIX}"
  if test "${NEXUS_FOUND}" = "1"; then
    echo "   NeXus found in $(pkg-config --variable=prefix nexus)"
  else
    echo "   NeXus found in ${NEXUS_PREFIX}"
  fi
else
  echo "   HDF4 not needed (NeXus support disabled)"
  echo "   HDF5 not needed (NeXus support disabled)"
  echo "   NeXus not needed (NeXus support disabled)"
fi
echo ""
if test "${BUILD_CUBA}" = "1"; then
  echo "   CUBA will be built"
elif test "${BUILD_CUBA}" != "1" && test "${BUILD_BMW_LIBS}" -eq 1; then
  if test "${CUBA_FOUND}" = "1"; then
    echo "   CUBA found in $(pkg-config --variable=prefix cuba)"
  else
    echo "   CUBA found in ${CUBA_PREFIX}"
  fi
else
  echo "   CUBA not needed (BMWlibs disabled)"
fi
echo ""
if test "${enable_editor}" != "xno"; then
  if test "x${QMAKEBIN}" != "x"; then
    echo "   Qt qmake found at ${QMAKEBIN}"
  else
    echo "   Qt qmake not found"
  fi
else
  echo "   Qt not needed (Qt editors disabled)"
fi
echo ""
echo ""
echo "   Features:"
echo "   ---------"
echo ""
echo "   musrfit (including musrfit, musrview, musrt0, musrparam,"
echo "            msr2msr, msr2data, any2many): yes"
echo ""
echo "   Supported muSR file formats:"
echo "     MUD:                                 yes"
echo "     PSI-BIN:                             yes"
echo "     PSI-MDU:                             yes"
echo "     WKM:                                 yes"
echo "     ROOT (LEM):                          yes"
if test "${PNEXUS_ENABLED}" -eq 1; then
  echo "     NeXus:                               yes"
else
  echo "     NeXus:                               no"
fi
echo ""
echo "   External user-function libraries:"
if test "${BUILD_AS_LIBS}" -eq 1; then
  echo "     ASlibs:                              yes"
else
  echo "     ASlibs:                              no"
fi
if test "${BUILD_BMW_LIBS}" -eq 1; then
  echo "     BMWlibs:                             yes"
else
  echo "     BMWlibs:                             no"
fi
echo ""
echo "   Qt musrfit editors:"
if test "x$enable_editor" != "xno" && test "x${QMAKEBIN}" != "x" && test "x${QTEDITOR}" = "xmusredit"; then
  echo "     musredit (Qt4):                      yes"
else
  echo "     musredit (Qt4):                      no"
fi
if test "x$enable_editor" != "xno" && test "x${QMAKEBIN}" != "x" && test "x${QTEDITOR}" = "xmusrgui"; then
  echo "     musrgui (Qt3):                       yes"
else
  echo "     musrgui (Qt3):                       no"
fi
echo ""
echo ""
echo "   Installation directories:"
echo "   -------------------------"
echo ""
echo "   Programs and XML configuration files:  ${INSTALLDIR}/bin"
echo "   Documentation:                         ${DOCDIR}"
echo ""

