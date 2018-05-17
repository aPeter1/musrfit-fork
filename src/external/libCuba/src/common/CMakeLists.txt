#--- workerini ----------------------------------------------------------------

#--- obj lib creation ---------------------------------------------------------
add_library(workerini OBJECT 
  WorkerIni.c
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  workerini BEFORE PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

#--- check for pre compile flags ----------------------------------------------
set(COMP_DEF "-DHAVE_CONFIG_H")
set(COMP_DEF ${COMP_DEF} "-DNOUNDERSCORE")

target_compile_definitions(
  workerini PRIVATE ${COMP_DEF}
)

