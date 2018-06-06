#--- cuhre --------------------------------------------------------------------

#--- obj lib creation ---------------------------------------------------------
add_library(cuhre OBJECT 
  Cuhre.c
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  cuhre BEFORE PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
)

#--- check for pre compile flags ----------------------------------------------
set(COMP_DEF "-DHAVE_CONFIG_H")
set(COMP_DEF ${COMP_DEF} "-DNOUNDERSCORE")
set(COMP_DEF ${COMP_DEF} "-Dcubafun_=libCuhrecubafun_")

target_compile_definitions(
  cuhre PRIVATE ${COMP_DEF}
)

