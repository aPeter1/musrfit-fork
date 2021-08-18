#--- dump_nonlocal_field ------------------------------------------------------

project(dump_nonlocal_field VERSION 1.0.0 LANGUAGES C CXX)

#--- feed dnlf_config.h -----------------------------------------
set(HAVE_DNLF_CONFIG_H 1 CACHE INTERNAL "dnlf_config.h is available")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/dnlf_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/dnlf_config.h)

add_executable(dump_nonlocal_field ${GIT_REV_H} dump_nonlocal_field.cpp)
target_compile_options(dump_nonlocal_field BEFORE PRIVATE "-DHAVE_CONFIG_H" "-DHAVE_DNLF_CONFIG_H" "${HAVE_GIT_REV_H}")
target_include_directories(dump_nonlocal_field 
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
)
target_link_libraries(dump_nonlocal_field ${ROOT_LIBRARIES} ${MUSRFIT_LIBS} PNL_PippardFitter)

#--- installation info --------------------------------------------------------
install(
  TARGETS
    dump_nonlocal_field
  RUNTIME DESTINATION 
    bin
)



