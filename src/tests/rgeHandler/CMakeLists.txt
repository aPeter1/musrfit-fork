# - rgeHandlerTest
cmake_minimum_required(VERSION 3.17)

project(rgeHandlerTest VERSION 0.9 LANGUAGES CXX)

#--- check for ROOT -----------------------------------------------------------
find_package(ROOT 6.18 REQUIRED COMPONENTS Gui MathMore Minuit2 XMLParser)
if (ROOT_mathmore_FOUND)
  execute_process(COMMAND root-config --bindir OUTPUT_VARIABLE ROOT_BINDIR)
  string(STRIP ${ROOT_BINDIR} ROOT_BINDIR)
  execute_process(COMMAND root-config --version OUTPUT_VARIABLE ROOT_VERSION)
  string(STRIP ${ROOT_VERSION} ROOT_VERSION)
  message("-- Found ROOT: ${ROOT_BINDIR} (found version: ${ROOT_VERSION})")
  #---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
  include(${ROOT_USE_FILE})
endif (ROOT_mathmore_FOUND)

#--- all checks done -> feed config.h -----------------------------------------
set(HAVE_CONFIG_H 1 CACHE INTERNAL "config.h is available")
configure_file(${CMAKE_SOURCE_DIR}/cmake/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_executable(rgeHandlerTest main.cpp)
target_compile_options(rgeHandlerTest BEFORE PRIVATE "-DHAVE_CONFIG_H")
target_include_directories(rgeHandlerTest 
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}../../include/>
)
target_link_libraries(rgeHandlerTest ${ROOT_LIBRARIES} PRgeHandler)

#--- installation info --------------------------------------------------------
install(
  TARGETS
    rgeHandlerTest
  RUNTIME DESTINATION 
    bin
)


