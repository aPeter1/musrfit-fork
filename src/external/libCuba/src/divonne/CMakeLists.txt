#--- divonne ------------------------------------------------------------------

#--- obj lib creation ---------------------------------------------------------
add_library(divonne OBJECT
  Divonne.c
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  divonne BEFORE PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
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
  divonne PRIVATE ${COMP_DEF}
)

