#--- vegas --------------------------------------------------------------------

#--- obj lib creation ---------------------------------------------------------
add_library(vegas OBJECT
  Vegas.c
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  vegas BEFORE PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
)

#--- check for pre compile flags ----------------------------------------------
set(COMP_DEF "-DHAVE_CONFIG_H")
set(COMP_DEF ${COMP_DEF} "-DNOUNDERSCORE")

target_compile_definitions(
  vegas PRIVATE ${COMP_DEF}
)

