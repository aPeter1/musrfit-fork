# - cuba library --------------------------------------------------------------

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(CUBA_VERSION "1.0.0")
set(CUBA_LIBRARY_NAME "cuba")
configure_file("cuba.pc.in" "cuba.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(cuba SHARED
  cuhre/Cuhre.c
  divonne/Divonne.c
  suave/Suave.c
  vegas/Vegas.c
  common/WorkerIni.c
)
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  cuba BEFORE PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cuhre>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/divonne>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/suave>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vegas>
)

#--- check for pre compile flags ----------------------------------------------
set(COMP_DEF "")
if (HAVE_FORK)
  set(COMP_DEF ${COMP_DEF} "-DHAVE_FORK")
endif (HAVE_FORK)
if (HAVE_ALLOCA_H)
  set(COMP_DEF ${COMP_DEF} "-DHAVE_ALLOCA_H")
endif (HAVE_ALLOCA_H)
if (HAVE_SHMGET)
  set(COMP_DEF ${COMP_DEF} "-DHAVE_SHMGET")
endif (HAVE_SHMGET)

target_compile_definitions(
  cuba PRIVATE ${COMP_DEF}
)

