#--- divonne ------------------------------------------------------------------

#--- obj lib creation ---------------------------------------------------------
add_library(divonne OBJECT
  Divonne.c
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  divonne BEFORE PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
)

#--- check for pre compile flags ----------------------------------------------
set(COMP_DEF "-DHAVE_CONFIG_H")
set(COMP_DEF ${COMP_DEF} "-DNOUNDERSCORE")
set(COMP_DEF ${COMP_DEF} "-Dcubafun_=libDivonnecubafun_")

target_compile_definitions(
  divonne PRIVATE ${COMP_DEF}
)

set(COMP_OPT "-O3")
set(COMP_OPT ${COMP_OPT} "-fomit-frame-pointer")
set(COMP_OPT ${COMP_OPT} "-ffast-math")
set(COMP_OPT ${COMP_OPT} "-fPIC")
target_compile_options(
  divonne PRIVATE ${COMP_OPT}
)
