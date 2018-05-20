#--- common -------------------------------------------------------------------

set(common_src Global.c)
set(common_src ${common_src} Data.c)
if (HAVE_FORK)
  set(common_src ${common_src} Fork.c)
endif (HAVE_FORK)

#--- obj lib creation ---------------------------------------------------------
add_library(common OBJECT ${common_src})

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  common BEFORE PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

#--- check for pre compile flags ----------------------------------------------
set(COMP_DEF "-DHAVE_CONFIG_H")
set(COMP_DEF ${COMP_DEF} "-DNOUNDERSCORE")

target_compile_definitions(
  common PRIVATE ${COMP_DEF}
)

set(COMP_OPT "-O3")
set(COMP_OPT ${COMP_OPT} "-fomit-frame-pointer")
set(COMP_OPT ${COMP_OPT} "-ffast-math")
target_compile_options(
  common PRIVATE ${COMP_OPT}
)
